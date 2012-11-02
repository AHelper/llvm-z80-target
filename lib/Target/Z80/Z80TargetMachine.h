//===-- Z80TargetMachine.h - Define TargetMachine for Z80 ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Z80 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef Z80TARGETMACHINE_H
#define Z80TARGETMACHINE_H

#include "Z80InstrInfo.h"
#include "Z80ISelLowering.h"
#include "Z80FrameLowering.h"
#include "Z80SelectionDAGInfo.h"
#include "Z80Subtarget.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {

class Z80TargetMachine : public LLVMTargetMachine {
  Z80Subtarget Subtarget;
  const TargetData DataLayout;       // Calculates type size & alignment
  Z80TargetLowering TLInfo;
  Z80SelectionDAGInfo TSInfo;
  Z80InstrInfo InstrInfo;
  Z80FrameLowering FrameLowering;
public:
  Z80TargetMachine(const Target &T, StringRef TT,
                     StringRef CPU, StringRef FS, const TargetOptions &Options,
                     Reloc::Model RM, CodeModel::Model CM,
                     CodeGenOpt::Level OL, bool hasUndoc);

  virtual const Z80InstrInfo *getInstrInfo() const { return &InstrInfo; }
  virtual const TargetFrameLowering  *getFrameLowering() const {
    return &FrameLowering;
  }
  virtual const Z80Subtarget   *getSubtargetImpl() const{ return &Subtarget; }
  virtual const Z80RegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }
  virtual const Z80TargetLowering* getTargetLowering() const {
    return &TLInfo;
  }
  virtual const Z80SelectionDAGInfo* getSelectionDAGInfo() const {
    return &TSInfo;
  }
  virtual const TargetData       *getTargetData() const { return &DataLayout; }

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
};

/// Z80V8TargetMachine - Z80 with undocumented instructions target machine
///
class Z80UTargetMachine : public Z80TargetMachine {
  virtual void anchor();
public:
  Z80UTargetMachine(const Target &T, StringRef TT,
                       StringRef CPU, StringRef FS,
                       const TargetOptions &Options,
                       Reloc::Model RM, CodeModel::Model CM,
                       CodeGenOpt::Level OL);
};

/// Z80NoUndocTargetMachine - Z80 with only documented instructions target machine
///
class Z80NoUndocTargetMachine : public Z80TargetMachine {
  virtual void anchor();
public:
  Z80NoUndocTargetMachine(const Target &T, StringRef TT,
                       StringRef CPU, StringRef FS,
                       const TargetOptions &Options,
                       Reloc::Model RM, CodeModel::Model CM,
                       CodeGenOpt::Level OL);
};

} // end namespace llvm

#endif
