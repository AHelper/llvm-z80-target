//===-- Z80TargetMachine.h - Define TargetMachine for Sparc ---*- C++ -*-===//
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

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "Z80InstrInfo.h"
#include "Z80Subtarget.h"
#include "Z80ISelLowering.h"

namespace llvm {

class Z80TargetMachine : public LLVMTargetMachine {


		Z80InstrInfo InstrInfo;

		
	public:

		virtual const Z80InstrInfo *getInstrInfo() const { return &InstrInfo; }

		virtual const Z80RegisterInfo *getRegisterInfo() const {
			return &InstrInfo.getRegisterInfo();
		}
		virtual const TargetData *getTargetData() const { return &DataLayout; }

};



} // end namespace llvm

#endif
