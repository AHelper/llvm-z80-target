//===-- Z80TargetMachine.cpp - Define TargetMachine for Z80 -----------===//
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

#include "Z80TargetMachine.h"
#include "Z80.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeZ80Target() {
  // Register the target.
  RegisterTargetMachine<Z80UTargetMachine> X(TheZ80UTarget);
  RegisterTargetMachine<Z80NoUndocTargetMachine> Y(TheZ80NUTarget);
}

/// Z80TargetMachine ctor - Create an ILP32 architecture model
///
Z80TargetMachine::Z80TargetMachine(const Target &T, StringRef TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Reloc::Model RM, CodeModel::Model CM,
                                       CodeGenOpt::Level OL,
                                       bool hasUndoc)
  : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
    Subtarget(TT, CPU, FS, hasUndoc),
    DataLayout(Subtarget.getDataLayout()),
    TLInfo(*this), TSInfo(*this), InstrInfo(Subtarget),
    FrameLowering(Subtarget) {
}

namespace {
/// Z80 Code Generator Pass Configuration Options.
class Z80PassConfig : public TargetPassConfig {
public:
  Z80PassConfig(Z80TargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  Z80TargetMachine &getZ80TargetMachine() const {
    return getTM<Z80TargetMachine>();
  }

  virtual bool addInstSelector();
  virtual bool addPreEmitPass();
};
} // namespace

TargetPassConfig *Z80TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Z80PassConfig(this, PM);
}

bool Z80PassConfig::addInstSelector() {
  PM->add(createZ80ISelDag(getZ80TargetMachine()));
  return false;
}

/// addPreEmitPass - This pass may be implemented by targets that want to run
/// passes immediately before machine code is emitted.  This should return
/// true if -print-machineinstrs should print out the code after the passes.
bool Z80PassConfig::addPreEmitPass(){
  PM->add(createZ80FPMoverPass(getZ80TargetMachine()));
  PM->add(createZ80DelaySlotFillerPass(getZ80TargetMachine()));
  return true;
}

void Z80UTargetMachine::anchor() { }

Z80UTargetMachine::Z80UTargetMachine(const Target &T,
                                           StringRef TT, StringRef CPU,
                                           StringRef FS,
                                           const TargetOptions &Options,
                                           Reloc::Model RM,
                                           CodeModel::Model CM,
                                           CodeGenOpt::Level OL)
  : Z80TargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, true) {
}

void Z80NoUndocTargetMachine::anchor() { }

Z80NoUndocTargetMachine::Z80NoUndocTargetMachine(const Target &T,
                                           StringRef TT,  StringRef CPU,
                                           StringRef FS,
                                           const TargetOptions &Options,
                                           Reloc::Model RM,
                                           CodeModel::Model CM,
                                           CodeGenOpt::Level OL)
  : Z80TargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false) {
}
