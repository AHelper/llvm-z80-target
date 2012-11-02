//===- Z80RegisterInfo.td - Z80 Register defs ----------*- tablegen -*-===//
//
// The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declarations that describe the Z80 register file
//===----------------------------------------------------------------------===//

class Z80Reg<string n> : Register<n> {
  field bits<5> Num;
  let Namespace = "Z80";
}
//===----------------------------------------------------------------------===//
// 8-bit Registers

def A : Z80Reg<"A">; // Accumulator
def B : Z80Reg<"B">;
def C : Z80Reg<"C">;
def D : Z80Reg<"D">;
def E : Z80Reg<"E">;
def H : Z80Reg<"H">;
def L : Z80Reg<"L">;

//===----------------------------------------------------------------------===//
// 8-bit Special Registers

def F : Z80Reg<"F">; // Flag register 
def I : Z80Reg<"I">; // Interrupt register
def R : Z80Reg<"R">; // Refresh register

def IXH : Z80Reg<"IXH">;
def IXL : Z80Reg<"IXL">;
def IYH : Z80Reg<"IYH">;
def IYL : Z80Reg<"IYL">;


//===----------------------------------------------------------------------===//
// 16-bit Registers

def BC : Z80Reg<"BC"> {
let SubRegs = [B,C];
}
def DE : Z80Reg<"DE"> {
let SubRegs = [D,E];
}
def HL : Z80Reg<"HL">{
let SubRegs = [H,L];
}
//===----------------------------------------------------------------------===//
// 16-bit Shadow Registers

def BCP : Z80Reg<"BC'">;
def DEP : Z80Reg<"DE'">;
def HLP : Z80Reg<"HL'">;

//===----------------------------------------------------------------------===//
// 16-bit Index Registers

def IX : Z80Reg<"IX">{
let SubRegs = [IXH,IXL]; // unofficially there are 8bit subregs accessible
}

def IY : Z80Reg<"IY">{
let SubRegs = [IYH,IYL];
}

//===----------------------------------------------------------------------===//
// 16-bit Special Registers

def SP : Z80Reg<"SP">; // Stack Pointer
def PC : Z80Reg<"PC">; // Program Counter
def AF : Z80Reg<"AF">{
let SubRegs = [A,F];
}
def AFP : Z80Reg<"AF'">;



// General purpose register, the accumulator
def ACC : RegisterClass<"Z80", [i8], 8, [A]>; // The only register you can perform arithmetic on

def GPR : RegisterClass<"Z80", [i8], 8, [B,C,D,E,H,L]>;

def IDX8 : RegisterClass<"Z80", [i8], 8, [IXH, IXY, IYH, IYL]>;
def INT8 : RegisterClass<"Z80", [i8], 8, [I]>;
def MEM8 : RegisterClass<"Z80", [i8], 8, [R]>;

def GPR16 : RegisterClass<"Z80", [i16], 8, [BC,DE,HL]>;
def SHD16 : RegisterClass<"Z80", [i16], 8, [BCP,DEP,HLP]>;
def ACF16 : RegisterClass<"Z80", [i16], 8, [AF, AFP]>;

def IDX16 : RegisterClass<"Z80", [i16], 8, [IX, IY]>;

def PC16 : RegisterClass<"Z80", [i16], 8, [PC]>;
def SP16 : RegisterClass<"Z80", [i16], 8, [SP]>;

