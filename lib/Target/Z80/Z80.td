//===- Sparc.td - Describe the Sparc Target Machine -------------*- C++ -*-===//
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

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"


//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "Z80RegisterInfo.td"
include "Z80CallingConv.td"
include "Z80InstrInfo.td"




def Z80InstrInfo : InstrInfo {} 

def Z80 : Target {
  let InstructionSet = Z80InstrInfo;
}
