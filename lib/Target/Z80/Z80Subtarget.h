//===-- Z80Subtarget.h - Define Subtarget for the Z80 -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Z80 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef Z80_SUBTARGET_H
#define Z80_SUBTARGET_H

#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "Z80GenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class Z80Subtarget : public Z80GenSubtargetInfo {
  virtual void anchor();
  bool HasUndoc;
  
public:
  Z80Subtarget(const std::string &TT, const std::string &CPU,
                 const std::string &FS, bool hasUndoc);

  bool useUndocumentedInstructions() const { return !HasNoUndoc; }
  
  /// ParseSubtargetFeatures - Parses features string setting specified 
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);
  
  std::string getDataLayout() const {
    const char *p;
    p = "e-S8-p:16:8:8-i8:8:8-i16:8:8-i32:8:8-n8:16";
    return std::string(p);
  }
};

} // end namespace llvm

#endif