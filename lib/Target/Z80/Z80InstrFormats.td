//===- PIC16InstrFormats.td - PIC16 Instruction Formats-------*- tblgen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe Z80 instructions format
//
//  All the possible PIC16 fields are:
//
//  opcode  		- operation code.
//  prefix  		- prefix bytes
//  displacement    - displacement byte
//  imm				- immediate data
//


//===----------------------------------------------------------------------===//
// Z80 Instruction Format Definitions. (copied from x86)
//

// ImmType - This specifies the immediate type used by an instruction. This is
// part of the ad-hoc solution used to emit machine instruction encodings by our
// machine code emitter.
class ImmType<bits<3> val> {
	bits<2> Value = val;
}
def NoImm      : ImmType<0>;
def Imm8       : ImmType<1>;
def Imm16      : ImmType<2>;

//===----------------------------------------------------------------------===//
// Generic Z80 Format
// Z80 instructions have variable length
// opcode = 8 bits


class Z80Inst<bits<8> opcod, dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {
	let Namespace = "Z80";

	bits<8> Opcode = opcod;			// General, because every Instruction has an opcode
	bits<16> Prefix = p;				// Prefix, either none, 8bit or 16bit
	bits<8> Displacement = d;			// Displacement byte
	bits<16> Immediate = imm;			// immediate, either none, 8bit or 16bit  

	dag OutOperandList = outs;
	dag InOperandList = ins;
	let AsmString = asmstr;
	let Pattern = pattern;
  
}


//===----------------------------------------------------------------------===//
// Unprefixed Instructions
// Comment: probably bad to do it this way


class NoPrefixInst<bits<6> opcode, dag outs, dag ins, string asmstr, list<dag> pattern> : Z80Inst<outs, ins, asmstr, pattern> {


}


