//===-- SparcTargetMachine.cpp - Define TargetMachine for Sparc -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//#include "SparcMCAsmInfo.h"
#include "Z80TargetMachine.h"
//#include "Sparc.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetRegistry.h"

using namespace llvm;

extern "C" void LLVMInitializeZ80Target() {
  // Register the target.
  RegisterTargetMachine<Z80TargetMachine> X(TheZ80Target);


}


Z80TargetMachine::Z80TargetMachine(const Target &T, const std::string &TT, const std::string &FS, bool is64bit)
  : LLVMTargetMachine(T, TT),
    Subtarget(TT, FS),
    DataLayout("e-p:16:8:8-i8:8:8-i16:8:8-i32:8:8-n8"),
    TLInfo(*this), 
    InstrInfo(*this),
    FrameInfo(TargetFrameInfo::StackGrowsDown, 8, 0) {
    
}

bool Z80TargetMachine::addInstSelector(PassManagerBase &PM, CodeGenOpt::Level OptLevel) {
  PM.add(createZ80ISelDag(*this));
  return false;
}


bool Z80TargetMachine::addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel){

  return true;
}


