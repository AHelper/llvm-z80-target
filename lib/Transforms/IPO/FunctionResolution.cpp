//===- FunctionResolution.cpp - Resolve declarations to implementations ---===//
//
// Loop over the functions that are in the module and look for functions that
// have the same name.  More often than not, there will be things like:
//
//    declare void %foo(...)
//    void %foo(int, int) { ... }
//
// because of the way things are declared in C.  If this is the case, patch
// things up.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/IPO.h"
#include "llvm/Module.h"
#include "llvm/SymbolTable.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Pass.h"
#include "llvm/iOther.h"
#include "llvm/Constants.h"
#include "llvm/Assembly/Writer.h"  // FIXME: remove when varargs implemented
#include "Support/Statistic.h"
#include <algorithm>

namespace {
  Statistic<>NumResolved("funcresolve", "Number of varargs functions resolved");
  Statistic<> NumGlobals("funcresolve", "Number of global variables resolved");

  struct FunctionResolvingPass : public Pass {
    bool run(Module &M);
  };
  RegisterOpt<FunctionResolvingPass> X("funcresolve", "Resolve Functions");
}

Pass *createFunctionResolvingPass() {
  return new FunctionResolvingPass();
}

static bool ResolveFunctions(Module &M, std::vector<GlobalValue*> &Globals,
                             Function *Concrete) {
  bool Changed = false;
  for (unsigned i = 0; i != Globals.size(); ++i)
    if (Globals[i] != Concrete) {
      Function *Old = cast<Function>(Globals[i]);
      const FunctionType *OldMT = Old->getFunctionType();
      const FunctionType *ConcreteMT = Concrete->getFunctionType();
      
      if (OldMT->getParamTypes().size() > ConcreteMT->getParamTypes().size() &&
          !ConcreteMT->isVarArg())
        if (!Old->use_empty()) {
          std::cerr << "WARNING: Linking function '" << Old->getName()
                    << "' is causing arguments to be dropped.\n";
          std::cerr << "WARNING: Prototype: ";
          WriteAsOperand(std::cerr, Old);
          std::cerr << " resolved to ";
          WriteAsOperand(std::cerr, Concrete);
          std::cerr << "\n";
        }
      
      // Check to make sure that if there are specified types, that they
      // match...
      //
      unsigned NumArguments = std::min(OldMT->getParamTypes().size(),
                                       ConcreteMT->getParamTypes().size());

      if (!Old->use_empty() && !Concrete->use_empty())
        for (unsigned i = 0; i < NumArguments; ++i)
          if (OldMT->getParamTypes()[i] != ConcreteMT->getParamTypes()[i]) {
            std::cerr << "WARNING: Function [" << Old->getName()
                      << "]: Parameter types conflict for: '" << OldMT
                      << "' and '" << ConcreteMT << "'\n";
            return Changed;
          }
      
      // Attempt to convert all of the uses of the old function to the concrete
      // form of the function.  If there is a use of the fn that we don't
      // understand here we punt to avoid making a bad transformation.
      //
      // At this point, we know that the return values are the same for our two
      // functions and that the Old function has no varargs fns specified.  In
      // otherwords it's just <retty> (...)
      //
      Value *Replacement = Concrete;
      if (Concrete->getType() != Old->getType())
        Replacement = ConstantExpr::getCast(ConstantPointerRef::get(Concrete),
                                            Old->getType());
      NumResolved += Old->use_size();
      Old->replaceAllUsesWith(Replacement);

      // Since there are no uses of Old anymore, remove it from the module.
      M.getFunctionList().erase(Old);
    }
  return Changed;
}


static bool ResolveGlobalVariables(Module &M,
                                   std::vector<GlobalValue*> &Globals,
                                   GlobalVariable *Concrete) {
  bool Changed = false;
  assert(isa<ArrayType>(Concrete->getType()->getElementType()) &&
         "Concrete version should be an array type!");

  // Get the type of the things that may be resolved to us...
  const ArrayType *CATy =cast<ArrayType>(Concrete->getType()->getElementType());
  const Type *AETy = CATy->getElementType();

  Constant *CCPR = ConstantPointerRef::get(Concrete);

  for (unsigned i = 0; i != Globals.size(); ++i)
    if (Globals[i] != Concrete) {
      GlobalVariable *Old = cast<GlobalVariable>(Globals[i]);
      const ArrayType *OATy = cast<ArrayType>(Old->getType()->getElementType());
      if (OATy->getElementType() != AETy || OATy->getNumElements() != 0) {
        std::cerr << "WARNING: Two global variables exist with the same name "
                  << "that cannot be resolved!\n";
        return false;
      }

      Old->replaceAllUsesWith(ConstantExpr::getCast(CCPR, Old->getType()));

      // Since there are no uses of Old anymore, remove it from the module.
      M.getGlobalList().erase(Old);

      ++NumGlobals;
      Changed = true;
    }
  return Changed;
}

static bool ProcessGlobalsWithSameName(Module &M,
                                       std::vector<GlobalValue*> &Globals) {
  assert(!Globals.empty() && "Globals list shouldn't be empty here!");

  bool isFunction = isa<Function>(Globals[0]);   // Is this group all functions?
  GlobalValue *Concrete = 0;  // The most concrete implementation to resolve to

  assert((isFunction ^ isa<GlobalVariable>(Globals[0])) &&
         "Should either be function or gvar!");

  for (unsigned i = 0; i != Globals.size(); ) {
    if (isa<Function>(Globals[i]) != isFunction) {
      std::cerr << "WARNING: Found function and global variable with the "
                << "same name: '" << Globals[i]->getName() << "'.\n";
      return false;                 // Don't know how to handle this, bail out!
    }

    if (isFunction) {
      // For functions, we look to merge functions definitions of "int (...)"
      // to 'int (int)' or 'int ()' or whatever else is not completely generic.
      //
      Function *F = cast<Function>(Globals[i]);
      if (!F->isExternal()) {
        if (Concrete && !Concrete->isExternal())
          return false;   // Found two different functions types.  Can't choose!
        
        Concrete = Globals[i];
      } else if (Concrete) {
        if (Concrete->isExternal()) // If we have multiple external symbols...x
          if (F->getFunctionType()->getNumParams() > 
              cast<Function>(Concrete)->getFunctionType()->getNumParams())
            Concrete = F;  // We are more concrete than "Concrete"!

      } else {
        Concrete = F;
      }
    } else {
      // For global variables, we have to merge C definitions int A[][4] with
      // int[6][4].  A[][4] is represented as A[0][4] by the CFE.
      GlobalVariable *GV = cast<GlobalVariable>(Globals[i]);
      if (!isa<ArrayType>(GV->getType()->getElementType())) {
        Concrete = 0;
        break;  // Non array's cannot be compatible with other types.
      } else if (Concrete == 0) {
        Concrete = GV;
      } else {
        // Must have different types... allow merging A[0][4] w/ A[6][4] if
        // A[0][4] is external.
        const ArrayType *NAT = cast<ArrayType>(GV->getType()->getElementType());
        const ArrayType *CAT =
          cast<ArrayType>(Concrete->getType()->getElementType());

        if (NAT->getElementType() != CAT->getElementType()) {
          Concrete = 0;  // Non-compatible types
          break;
        } else if (NAT->getNumElements() == 0 && GV->isExternal()) {
          // Concrete remains the same
        } else if (CAT->getNumElements() == 0 && Concrete->isExternal()) {
          Concrete = GV;   // Concrete becomes GV
        } else {
          Concrete = 0;    // Cannot merge these types...
          break;
        }
      }
    }
    ++i;
  }

  if (Globals.size() > 1) {         // Found a multiply defined global...
    // If there are no external declarations, and there is at most one
    // externally visible instance of the global, then there is nothing to do.
    //
    bool HasExternal = false;
    unsigned NumInstancesWithExternalLinkage = 0;

    for (unsigned i = 0, e = Globals.size(); i != e; ++i) {
      if (Globals[i]->isExternal())
        HasExternal = true;
      else if (!Globals[i]->hasInternalLinkage())
        NumInstancesWithExternalLinkage++;
    }
    
    if (!HasExternal && NumInstancesWithExternalLinkage <= 1)
      return false;  // Nothing to do?  Must have multiple internal definitions.


    // We should find exactly one concrete function definition, which is
    // probably the implementation.  Change all of the function definitions and
    // uses to use it instead.
    //
    if (!Concrete) {
      std::cerr << "WARNING: Found global types that are not compatible:\n";
      for (unsigned i = 0; i < Globals.size(); ++i) {
        std::cerr << "\t" << Globals[i]->getType()->getDescription() << " %"
                  << Globals[i]->getName() << "\n";
      }
      std::cerr << "  No linkage of globals named '" << Globals[0]->getName()
                << "' performed!\n";
      return false;
    }

    if (isFunction)
      return ResolveFunctions(M, Globals, cast<Function>(Concrete));
    else
      return ResolveGlobalVariables(M, Globals,
                                    cast<GlobalVariable>(Concrete));
  }
  return false;
}

bool FunctionResolvingPass::run(Module &M) {
  SymbolTable &ST = M.getSymbolTable();

  std::map<std::string, std::vector<GlobalValue*> > Globals;

  // Loop over the entries in the symbol table. If an entry is a func pointer,
  // then add it to the Functions map.  We do a two pass algorithm here to avoid
  // problems with iterators getting invalidated if we did a one pass scheme.
  //
  for (SymbolTable::iterator I = ST.begin(), E = ST.end(); I != E; ++I)
    if (const PointerType *PT = dyn_cast<PointerType>(I->first)) {
      SymbolTable::VarMap &Plane = I->second;
      for (SymbolTable::type_iterator PI = Plane.begin(), PE = Plane.end();
           PI != PE; ++PI) {
        GlobalValue *GV = cast<GlobalValue>(PI->second);
        assert(PI->first == GV->getName() &&
               "Global name and symbol table do not agree!");
        Globals[PI->first].push_back(GV);
      }
    }

  bool Changed = false;

  // Now we have a list of all functions with a particular name.  If there is
  // more than one entry in a list, merge the functions together.
  //
  for (std::map<std::string, std::vector<GlobalValue*> >::iterator
         I = Globals.begin(), E = Globals.end(); I != E; ++I)
    Changed |= ProcessGlobalsWithSameName(M, I->second);

  // Now loop over all of the globals, checking to see if any are trivially
  // dead.  If so, remove them now.

  for (Module::iterator I = M.begin(), E = M.end(); I != E; )
    if (I->isExternal() && I->use_empty()) {
      Function *F = I;
      ++I;
      M.getFunctionList().erase(F);
      ++NumResolved;
      Changed = true;
    } else {
      ++I;
    }

  for (Module::giterator I = M.gbegin(), E = M.gend(); I != E; )
    if (I->isExternal() && I->use_empty()) {
      GlobalVariable *GV = I;
      ++I;
      M.getGlobalList().erase(GV);
      ++NumGlobals;
      Changed = true;
    } else {
      ++I;
    }

  return Changed;
}
