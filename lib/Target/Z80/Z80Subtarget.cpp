//===-- Z80Subtarget.cpp - Z80 Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Z80 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "Z80Subtarget.h"
#include "Z80.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Z80GenSubtargetInfo.inc"

using namespace llvm;

void Z80Subtarget::anchor() { }

Z80Subtarget::Z80Subtarget(const std::string &TT, const std::string &CPU,
                               const std::string &FS,  bool hasUndoc) :
  Z80GenSubtargetInfo(TT, CPU, FS),
  HasUndoc(hasUndoc) {
  
  // Determine default and user specified characteristics
  std::string CPUName = CPU;
  if (CPUName.empty()) {
    if (hasUndoc)
      CPUName = "NU";
    else
      CPUName = "U";
  }

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
}
