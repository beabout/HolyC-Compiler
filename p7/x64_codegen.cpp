#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace holeyc{

void IRProgram::allocGlobals(){
	// TODO(Implement me)
  // CLB: give a label to each global 
  // ex.  g1 myLoc: gbl-g1
  if(!globals.empty()){
    for (auto it = globals.begin(); it != globals.end(); ++it){
      // gotta do something with these
      it->first;  // SemSymbol
      it->second; // SymOpd
    }
  }
}

void IRProgram::datagenX64(std::ostream& out){
	TODO(Write out data section)
  out << ".data\n";
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
  
  // write out .data here 
  // Drew: "Make sure your code is x64 aligned."
	out << ".align 8\n";
}

void IRProgram::toX64(std::ostream& out){
	// for auto x in procedures
	// for auto y in quads ( this happens inside procedure->toX64(out) )
	// quad->toX64 
	for(auto procedure : this->procs){
		procedure->toX64(out);
	}
}

void Procedure::allocLocals(){
  if (!locals.empty()){
    for (auto it = locals.begin(); it != locals.end(); ++it){
      it->first;  // SemSymbol
      it->second; // SymOpd
    }
  }
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();

	enter->codegenLabels(out);
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

void BinOpQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
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
	TODO(Implement me)
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

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

void CallQuad::codegenX64(std::ostream& out){
	out << "callq " << "fn_" << this->callee->getName() << "\n"; // i think - Evan
}

void EnterQuad::codegenX64(std::ostream& out){
//	    pushq %rbp			# we always do this on enter fn.
//		movq %rsp, %rbp		# we always do this on enter fn.
//		addq $16, %rbp		# we always do this on enter fn.
	out << "pushq %rbp\n";
	out << "movq %rsp, %rbp\n";
	out << "addq $16, %rbp\n";

	//TODO(Implement me)
}

void LeaveQuad::codegenX64(std::ostream& out){
//	fn_leave_bar: 	add $8, %rsp # restore stack pointer // i think this may also be essentially popq
//				    popq %rbp	 # pop base pointer
//				    retq		 # return
	out << "add $8, %rsp\n"; // <- this may be popq instead, depends on oracle.
	out << "popq %rbp\n";
	out << "retq\n";
	//TODO(Implement me)
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
	TODO(Implement me)
	// i think we probably movq from memory to regStr
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	TODO(Implement me)
	// i think we probably movq from regStr to memory
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
	TODO(Implement me)
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	TODO(Implement me)
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}