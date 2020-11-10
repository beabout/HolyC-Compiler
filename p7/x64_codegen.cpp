#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc{

void IRProgram::allocGlobals(){
	// TODO(Implement me)
  // CLB: give a label to each global
  // ex.  For global g, we need to give it a label: 
  // - In .data section: gbl_g: .quad 0 
  // - In .text section: movq $4, (gbl_g)

  // GenStore takes a register, the contents of that register go into memory
  // i.e movq %rax, -24(%rbp)

  // GenLoad takes a register, to store some contents of memory in
  // i.e movq -24(%rbp), %rax
  
  if(!globals.empty()){
    for (auto it = globals.begin(); it != globals.end(); ++it){
      // - it->first  : SemSymbol *
      // - it->second : SymOpd *
      it->second->setMemoryLoc("gbl_" + it->first->getName()); // SymOpd
      // add these to memory. 
    }
  }

  if(!strings.empty()){
    for (auto it = strings.begin(); it != strings.end(); ++it){
      // - it->first  : AuxOpd * 
      // - it->second : string
      it->first->setMemoryLoc("str_" + it->first->getName());
      // add these to memory.
    }
  }
}

void IRProgram::datagenX64(std::ostream& out){
	// TODO(Write out data section)
  out << ".data\n";
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
  this->allocGlobals();
  // Drew: "Make sure your code is x64 aligned."
	out << ".align 8\n";
  out << ".globl main\n";
  out << "main: nop\n";
}

// THIS IS WHERE THE PROGRAM STARTS
void IRProgram::toX64(std::ostream& out){
	// for auto x in procedures
	// for auto y in quads ( this happens inside procedure->toX64(out) )
	// quad->toX64 
  this->datagenX64(out);
  
	for(auto procedure : this->procs){
		procedure->toX64(out);
	}
}

void Procedure::allocLocals(){
  // CLB: give a label to each local
  // don't forget to add the offset.
  int offset = -24; // CLB: Start at -24 because we need 16 bits for SBP and SIP
  if (!locals.empty()){
    for (auto it = locals.begin(); it != locals.end(); ++it){
      it->second->setMemoryLoc(std::to_string(offset) + "(%rbp)");
      offset = offset - 8;
    }
  }
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();
  
  out << "fun_" << myName << ":" << "\n";
	
  // enter->codegenLabels(out);
	enter->codegenX64(out);
	for (auto quad : *bodyQuads){
		quad->codegenLabels(out);
		quad->codegenX64(out);
	}
	leave->codegenLabels(out);
	leave->codegenX64(out);
}

void Quad::codegenLabels(std::ostream& out){
	if (labels.empty()){ return; }

	size_t numLabels = labels.size();
	size_t labelIdx = 0;
	for ( Label * label : labels){
		out << label->toString() << ": "; 
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
	}
}

// Covers ADD, SUB, DIV, MULT, OR, AND, EQ, NEQ, LT, GT, LTE, GTE 
void BinOpQuad::codegenX64(std::ostream &out){
  std::string bin_op_command = "";
  std::string reg_1  = "%rax";
  std::string reg_2  = "%rbx";
  // src1->genLoad(out, "thing");
  // src2->genLoad(out, "thing");
  // std::string movq_1 = "movq " + src1->getMemoryLoc() + ", " + reg_1 + "\n"; // load Opd1 into register
  // std::string movq_2 = "movq " + src2->getMemoryLoc() + ", " + reg_2 + "\n"; // load Opd2 into register

  src1->genLoad(out, reg_1);
  src2->genLoad(out, reg_2);
  switch (op)
  {
  case ADD:
    out << "addq " + reg_1 + ", " + reg_2 + "\n";
    dst->genStore(out, reg_2);
    //out << "movq " + reg_2 + ", " + dst->getMemoryLoc() + "\n";
    break;
  case SUB:
    out << "subq " + reg_1 + ", " + reg_2 + "\n";
    dst->genStore(out, reg_2);
    break;
  case DIV:
    out << "movq $0, %rdx\n";
    out << "idivq " + reg_2 + "\n";
    dst->genStore(out,reg_1);
    break;
  case MULT:
    out << "imulq " + reg_2 + "\n";
    dst->genStore(out,reg_1);
    break;
  case OR:
    out << "orq " + reg_1 + ", " + reg_2 + "\n";
    dst->genStore(out, reg_2);
    break;
  case AND:
    out << "andq " + reg_1 + ", " + reg_2 + "\n";
    dst->genStore(out, reg_2);
    break;
  case EQ:
    out << "sete %al\n"; // incomplete
    break;
  case NEQ:
    out << "setne %al\n"; // incomplete
    break;
  case LT:
    out << "setl %al\n"; // incomplete
    break;
  case GT:
    out << "setg %al\n"; // incomplete
    break;
  case LTE:
    out << "setle %al\n"; // incomplete
    break;
  case GTE:
    out << "setge %al\n"; // incomplete
    break;
  }

  //out << movq_1;
  //out << movq_2;
  //out << bin_op_command; // do the BinOp
}

// Covers Not, Neg
void UnaryOpQuad::codegenX64(std::ostream& out){
  // movq %rax, %rcx <-- what if --> movq -24(%rbp), %rcx
  // notq %rcx
  // movq %rcx, %r11
  std::string reg_1 = "%rcx";
  std::string command = "";
  src->genLoad(out, reg_1);
  //out << "movq " + src->getMemoryLoc() + ", " + reg_1 + "\n";
  switch(op){
    case NOT: 
      command += "notq";
      break; 
    case NEG: 
      command += "negq";
      break; 
  }
  out << command + " " + reg_1 + "\n";
  dst->genStore(out, reg_1);
  //out << "movq " + reg_1 + ", " + dst->getMemoryLoc() + "\n"; // DO WE NEED TO DO THIS: store result into the destination Opd
}

void AssignQuad::codegenX64(std::ostream& out){
  // this->dst->getSym()->getLoc();
	src->genLoad(out, "%rax");
	dst->genStore(out, "%rax");
}

void LocQuad::codegenX64(std::ostream& out){
	// (Optional)
	TODO(Implement me)
}

void JmpQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	out << "jmpe " << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

// Covers ToConsole, FromConsole
void IntrinsicQuad::codegenX64(std::ostream& out){
	switch(myIntrinsic){
	case OUTPUT:
		myArg->genLoad(out, "%rdi");
		if (myArg->getWidth() == QUADWORD){
			out << "callq printInt\n";
		} else if (myArg->getWidth() == BYTE){
			out << "callq printByte\n";
		} else {
			//If the argument is an ADDR,
			// assume it's a string
			out << "callq printString";
		}
		break;
	case INPUT:
		TODO("IMPLEMENT ME");
	}
}

// void ToConsoleStmtNode::codegenx64(std::ostream& out)
// {
// 	// source:
// 	// https://stackoverflow.com/questions/27594297/how-to-print-a-string-to-the-terminal-in-x86-64-assembly-nasm-without-syscall
// 	///section .data
//     //string1 db  0xa, "  Hello StackOverflow!!!", 0xa, 0xa, 0
// 	out << ".data\n"; // put the string to print in the data section
// 	out << unique_name +  " db  0xa, " + this->mySrc->toString() + ", 0xa, 0xa, 0";
// 	out << ".text\n";
// 	out << "movq string, %rsi\n"; // # put the string in rsi
// 	out << "movq $1, %rax\n"; // # put 1 in rax to get the right syscall?
// 	out << "movq %rax, %rdi\n"; // # set destination to stdout
// 	out << "syscall\n"; // # call syscall
// 	TODO(Implement me)
// }

void CallQuad::codegenX64(std::ostream& out){
	out << "callq " << "fn_" << this->callee->getName() << "\n"; // i think - Evan
}

void EnterQuad::codegenX64(std::ostream& out){
  out << "pushq %rbp\n";
	out << "movq %rsp, %rbp\n";
	out << "addq $16, %rbp\n";
  out << "subq $" << this->myProc->localsSize() << ", %rsp\n";
}

void LeaveQuad::codegenX64(std::ostream& out){
//	fn_leave_bar: 	add $8, %rsp # restore stack pointer // i think this may also be essentially popq
//				    popq %rbp	 # pop base pointer
//				    retq		 # return
	out << "add $" << this->myProc->localsSize() << ", %rsp\n"; // 
	out << "popq %rbp\n";
	out << "retq\n";
}

void SetArgQuad::codegenX64(std::ostream& out){
	TODO(Implement me)

}

void GetArgQuad::codegenX64(std::ostream& out){
	// We don't actually need to do anything here
  // CLB: This would be a simple "movq" method which is probably 
  // already taken care of elsewhere.
}

void SetRetQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void GetRetQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
  out << "movq " + getMemoryLoc() + ", " + regStr << std::endl;
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
   out << "movq " + regStr + ", " + getMemoryLoc() << std::endl;
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
  out << "movq " + getMemoryLoc() + ", " + regStr << std::endl;
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
  out << "movq " + regStr + ", " + getMemoryLoc() << std::endl;
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
  out << "movq $" + valString() + ", " + regStr << std::endl;
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}