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
      it->second->setMemoryLoc("(gbl_" + it->first->getName() + ")"); // SymOpd
      // add these to memory. 
    }
  }

  if(!strings.empty()){
    for (auto it = strings.begin(); it != strings.end(); ++it){
      // - it->first  : AuxOpd * 
      // - it->second : string
      it->first->setMemoryLoc(it->first->getName());
    }
  }
}

void IRProgram::datagenX64(std::ostream& out){
  this->allocGlobals();
  out << ".data\n";
  // print out globals and strings here 
  // .asciz - use for strings
  // .quad - numbers 
  if(!strings.empty()){
    for (auto it = strings.begin(); it != strings.end(); ++it){
      out << it->first->getName() + ": .asciz " + it->second << std::endl;
    }
  }
  if (!globals.empty()){
    for (auto it = globals.begin(); it != globals.end(); ++it){
      out << "gbl_" + it->first->getName() + ": .quad 0" << std::endl;
    }
  }


  


  //Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
  // Drew: "Make sure your code is x64 aligned."
	out << ".align 8\n";  
  out << ".globl main\n";
  out << ".text\n";
  //out << "main: jmp fun_main\n";
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

  // Evan : Formals under heavy construction, masks and welding goggles required.
  if (!formals.empty()){
    int counter = 1;
    for (auto it = formals.begin(); it != formals.end(); ++it){
      std::cout << counter << std::endl; 
        switch(counter){
          case 1:
            (*it)->setMemoryLoc("%rdi");
            break;
          case 2:
            (*it)->setMemoryLoc("%rsi");
            break;
          case 3:
            (*it)->setMemoryLoc("%rdx");
            break;
          case 4:
            (*it)->setMemoryLoc("%rcx");
            break;
          case 5:
            (*it)->setMemoryLoc("%r8");
            break;
          case 6:             
            (*it)->setMemoryLoc("%r9");
            break;
          default:
            int i = -((counter - 1) * 8);
            std::string offset_register = std::to_string(i) + "(%rbp)";
            (*it)->setMemoryLoc(offset_register);
            break;       
        }
     counter++;
    }
  }

  if (!temps.empty()){
    for (auto it = temps.begin(); it != temps.end(); ++it){
      (*it)->setMemoryLoc(std::to_string(offset) + "(%rbp)");
      offset = offset - 8;
    }
  }
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();
  
  out << myName << ":" << "\n";
	
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
  std::string reg_1  = "%rax";
  std::string reg_2  = "%rbx";

  if (op == ADD || op == SUB || op == DIV || op == MULT){
    src1->genLoad(out, reg_1);
    src2->genLoad(out, reg_2);
  } else {
    src1->genLoad(out, reg_1, true);
    src2->genLoad(out, reg_2, true);
  }
  switch (op) {
  case ADD:
    out << "addq " + reg_1 + ", " + reg_2 + "\n";
    dst->genStore(out, reg_2);
    break;
  case SUB:
    // CLAY: Think these need to be flipped like this in order to work. 
    out << "subq " + reg_2 + ", " + reg_1 + "\n";
    dst->genStore(out, reg_1);
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
    out << "cmpq " + reg_1 + ", " + reg_2 + "\n";
    out << "sete %al\n";
    dst->genStore(out, "%al");
    break;
  case NEQ:
    out << "cmpq " + reg_1 + ", " + reg_2 + "\n";
    out << "setne %al\n";
    dst->genStore(out, "%al");
    break;
  case LT:
    out << "cmpq " + reg_2 + ", " + reg_1 + "\n";
    out << "setl %al\n";
    dst->genStore(out, "%al");
    break;
  case GT:
    out << "cmpq " + reg_2 + ", " + reg_1 + "\n";
    out << "setg %al\n";
    dst->genStore(out, "%al");
    break;
  case LTE:
    out << "cmpq " + reg_2 + ", " + reg_1 + "\n";
    out << "setle %al\n";
    dst->genStore(out, "%al");
    break;
  case GTE:
    out << "cmpq " + reg_2 + ", " + reg_1 + "\n";
    out << "setge %al\n";
    dst->genStore(out, "%al");
    break;
  }
}

// Covers Not, Neg
void UnaryOpQuad::codegenX64(std::ostream& out){
  // movq %rax, %rcx <-- what if --> movq -24(%rbp), %rcx
  // notq %rcx
  // movq %rcx, %r11
  std::string reg_1 = "%rcx";
  src->genLoad(out, reg_1);
  switch(op){
    case NOT: 
      out << "notq " + reg_1 + "\n";
      break; 
    case NEG: 
      out << "negq " + reg_1 + "\n";
      break; 
  }
  dst->genStore(out, reg_1);
}

void AssignQuad::codegenX64(std::ostream& out){
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
  cnd->genLoad(out, "%al");
  out << "cmpb $0, %al\n";
	out << "je " << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

// Covers ToConsole, FromConsole
void IntrinsicQuad::codegenX64(std::ostream& out){
	switch(myIntrinsic){
	case OUTPUT:
    
    myArg->genLoad(out, "%rdi"); // this overwrites rdi, which is bad.
    if (myArg->getWidth() == QUADWORD){
      out << "callq printInt\n";
    }
    else if (myArg->getWidth() == BYTE){
      out << "callq printByte\n";
    } else {
			//If the argument is an ADDR,
			// assume it's a string
			out << "callq printString\n";
		}
    
		break;
	case INPUT:
    if (myArg->getWidth() == QUADWORD){
      out << "callq getInt\n";
      myArg->genStore(out, "%rax");
    }
    if (myArg->getWidth() == BYTE){
      if(myArgsType == "char"){
        std::cout << "should be char " << myArgsType << std::endl; 
        out << "callq getChar\n";
      } else {
        std::cout << "should be bool " << myArgsType << std::endl;
        out << "callq getBool\n";
      }
      myArg->genStore(out, "%rax");
    }
	}
}

void CallQuad::codegenX64(std::ostream& out){

	out << "callq " << this->callee->getName() << "\n"; // i think - Evan

}

void EnterQuad::codegenX64(std::ostream& out){
  out << "pushq %rbp\n";
	out << "movq %rsp, %rbp\n";
	out << "addq $16, %rbp\n";
  out << "subq $" << this->myProc->localsSize() << ", %rsp\n";
}

void LeaveQuad::codegenX64(std::ostream& out){
	out << "addq $" << this->myProc->localsSize() << ", %rsp\n"; // 
	out << "popq %rbp\n";
	out << "retq\n";
}

void SetArgQuad::codegenX64(std::ostream& out){
	// TODO(Implement me)
  std::cout << "INSIDE A SET ARG\n";
  switch(index){
    case 1:
      opd->genLoad(out, "%rdi");
      std::cout << "did rdi \n";
      break;
    case 2:
      opd->genLoad(out, "%rsi");
      std::cout << "did rsi \n";
      break;
    case 3:
      opd->genLoad(out, "%rdx");
      std::cout << "did rdx \n";
      break;
    case 4:
      opd->genLoad(out, "%rcx");
      std::cout << "did rcx \n";
      break;
    case 5:
      opd->genLoad(out, "%r8");
      std::cout << "did r8 \n";
      break;
    case 6:             
      opd->genLoad(out, "%r9");
      std::cout << "did r9 \n";
      break;
    default:
      int i = -((index - 1) * 8);
      std::string offset_register = std::to_string(i) + "(%rbp)";
      opd->genLoad(out, offset_register);
      std::cout << "did default \n";
      break;       
  }
}

void GetArgQuad::codegenX64(std::ostream& out){
	// We don't actually need to do anything here
  // CLB: This would be a simple "movq" method. 
  // - If value is in stack, we don't need to do anything. 
  // - Heap values do need to be moved though. 
  //out << "movq %rdi, -60(%rbp)\n";
  //   switch(index){
  //   case 1:
  //     opd->genStore(out, "%rdi");
  //     std::cout << "did rdi \n";
  //     break;
  //   case 2:
  //     opd->genStore(out, "%rsi");
  //     std::cout << "did rsi \n";
  //     break;
  //   case 3:
  //     opd->genStore(out, "%rdx");
  //     std::cout << "did rdx \n";
  //     break;
  //   case 4:
  //     opd->genStore(out, "%rcx");
  //     std::cout << "did rcx \n";
  //     break;
  //   case 5:
  //     opd->genStore(out, "%r8");
  //     std::cout << "did r8 \n";
  //     break;
  //   case 6:             
  //     opd->genStore(out, "%r9");
  //     std::cout << "did r9 \n";
  //     break;
  //   default:
  //     int i = -((index - 1) * 8);
  //     std::string offset_register = std::to_string(i) + "(%rbp)";
  //     opd->genStore(out, offset_register);
  //     std::cout << "did " + offset_register + " \n";
  //     break;       
  // }
  
}

void SetRetQuad::codegenX64(std::ostream& out){
  // movq stuff, %rax
  this->opd->genLoad(out, "%rax");
}

void GetRetQuad::codegenX64(std::ostream& out){
  // mov %rax, stuff
  this->opd->genStore(out, "%rax");
}

void SymOpd::genLoad(std::ostream & out, std::string regStr, bool is_boolean){
  std::string thing = "";
  std::string mov_command = "movq"; 
  if(this->getWidth() == ADDR){
    thing += "$";
  }
  thing += getMemoryLoc();
  if(regStr == "%al" && this->getWidth() == BYTE){
    mov_command = "movb";
  }
  out << mov_command + " " + thing + ", " + regStr << std::endl;
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
  std::string thing = "";
  std::string mov_command = "movq";
  if (this->getWidth() == ADDR){
    thing += "$";
  }
  thing += getMemoryLoc();
  if (regStr == "%al" && this->getWidth() == BYTE){
    mov_command = "movb";
  }
   out << mov_command + " " + regStr + ", " + thing << std::endl;
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr, bool is_boolean){
  std::string thing = "";
  std::string mov_command = "movq"; 
  if(this->getWidth() == ADDR){
    thing += "$";
  }
  thing += getMemoryLoc();
  if(regStr == "%al" && this->getWidth() == BYTE){
    mov_command = "movb";
  }
  out << mov_command + " " + thing + ", " + regStr << std::endl;
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
  std::string thing = "";
  std::string mov_command = "movq";
  if (this->getWidth() == ADDR){
    thing += "$";
  }
  thing += getMemoryLoc();
  if (regStr == "%al" && this->getWidth() == BYTE){
    mov_command = "movb";
  }
  out << mov_command + " " + regStr + ", " + thing << std::endl;
}

void LitOpd::genLoad(std::ostream & out, std::string regStr, bool is_boolean){
  std::string mov_command = "movq"; 
  if(regStr == "%al"){
    mov_command = "movb";
  }
  std::string thing = "";
  if(this->getWidth() == BYTE && !is_boolean){
    // assume it's a char or bool  
    thing = std::to_string(int(valString()[0]));
    std::cout << "char or bool ";
  } else {
    // integer or string
    std::cout << "integer or string ";
    thing = valString();
  }
  std::cout << valString() << std::endl;
  out << mov_command + " $" + thing + ", " + regStr << std::endl;
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}