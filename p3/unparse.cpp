#include "ast.hpp"

namespace holeyc{

/*
doIndent is declared static, which means that it can 
only be called in this file (its symbol is not exported).
*/
static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

/*
In this code, the intention is that functions are grouped 
into files by purpose, rather than by class.
If you're used to having all of the functions of a class 
defined in the same file, this style may be a bit disorienting,
though it is legal. Thus, we can have
ProgramNode::unparse, which is the unparse method of ProgramNodes
defined in the same file as DeclNode::unparse, the unparse method
of DeclNodes.
*/


void ProgramNode::unparse(std::ostream& out, int indent){
	/* Oh, hey it's a for-each loop in C++!
	   The loop iterates over each element in a collection
	   without that gross i++ nonsense. 
	 */
	for (auto global : *myGlobals){
		/* The auto keyword tells the compiler
		   to (try to) figure out what the
		   type of a variable should be from 
		   context. here, since we're iterating
		   over a list of DeclNode *s, it's 
		   pretty clear that global is of 
		   type DeclNode *.
		*/
		global->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myType->unparse(out, 0);
	if(this->myType->isRef()){
		out<< "ptr";
	}
	out << " ";
	this->myId->unparse(out, 0);
	out << ";\n";
}

void IDNode::unparse(std::ostream& out, int indent){
	out << this->myStrVal;
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	out << "int";
}

// type id ( formalsList ) { stmtList }
void FnDeclNode::unparse(std::ostream& out, int indent){
	this->myType->unparse(out,0);
	out << " ";
	this->myId->unparse(out,0);
	out << "(";

  std::list<VarDeclNode*>::iterator it;
  std::list<StmtNode *>::iterator it2;
  for(it = this->formalDeclNodes->begin(); it != this->formalDeclNodes->end(); ++it){
		(*it)->unparse(out, 0);
	}
  out << "){";
  for (it2 = this->stmtNodes->begin(); it2 != this->stmtNodes->end(); ++it2){
    (*it2)->unparse(out, 0);
  }
  out << "}\n";
}

// void formalDeclNode::unparse(std::ostream &out, int indent)
// {

// }

void BoolTypeNode::unparse(std::ostream &out, int indent)
{
  out << "bool";
}

void CharTypeNode::unparse(std::ostream& out, int indent){
  out << "char";
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
  out << "void";
}

void NestedExpNode::unparse(std::ostream &out, int indent){
  out << "(";
  this->myExp->unparse(out,0);
  out << ")";
}

void IntLitNode::unparse(std::ostream &out, int indent){

}
void StrLitNode::unparse(std::ostream &out, int indent){

}
void NullPtrNode::unparse(std::ostream &out, int indent){

}
void CharLitNode::unparse(std::ostream &out, int indent){

}
void TrueNode::unparse(std::ostream &out, int indent){

}
void FalseNode::unparse(std::ostream &out, int indent){

}
void CallExpNode::unparse(std::ostream &out, int indent){

}

} // End namespace holeyc
