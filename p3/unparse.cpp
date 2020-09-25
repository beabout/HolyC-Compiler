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
//int main(int a, int b,<==)
// int a,
// int a 
// int b;
void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myType->unparse(out, indent);
	out << " ";
	this->myId->unparse(out, indent);
	if(! this->CommaDelimited){
		out << ";\n";
	}
}

void FormalVarDeclListNode::unparse(std::ostream& out, int indent){
  if(this->myVarDecls->size() > 0) {
    while(this->myVarDecls->size() > 1){
      this->myVarDecls->front()->unparse(out,indent); // VarDeclNode
      out << ", ";
      this->myVarDecls->pop_front();
    }
    this->myVarDecls->front()->unparse(out,indent);
  }
}

void IDNode::unparse(std::ostream& out, int indent){
  out << this->myStrVal;
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	out << "int";
  if(this->isRef()){
    out << "ptr";
  }
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	this->myType->unparse(out,indent);
	out << " ";
	this->myId->unparse(out,indent);
	out << "(";
  this->formalDeclListNode->unparse(out, indent);
  out << "){\n";
  std::list<StmtNode *>::iterator it2;
  for (it2 = this->stmtNodes->begin(); it2 != this->stmtNodes->end(); ++it2){
    (*it2)->unparse(out, indent+1);
  }
  out << "}\n";
}

// void formalDeclNode::unparse(std::ostream &out, int indent)
// {

// }

void BoolTypeNode::unparse(std::ostream &out, int indent)
{
  out << "bool";
  if(this->isRef()){
    out << "ptr";
  }
}

void CharTypeNode::unparse(std::ostream& out, int indent){
  out << "char";
  if(this->isRef()){
    out << "ptr";
  }
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
  out << "void";
}

void NestedExpNode::unparse(std::ostream &out, int indent){
  this->myExp->unparse(out,indent);
}

void IntLitNode::unparse(std::ostream &out, int indent){
  out << this->myInt;
}
void StrLitNode::unparse(std::ostream &out, int indent){
  out << this->myStrVal;
}
void NullPtrNode::unparse(std::ostream &out, int indent){
  out << "NULLPTR";
}
void CharLitNode::unparse(std::ostream &out, int indent){
  out << "'";
  out << this->myChar;

}
void TrueNode::unparse(std::ostream &out, int indent){
  out << "true";
}
void FalseNode::unparse(std::ostream &out, int indent){
  out << "false";
}
void CallExpNode::unparse(std::ostream &out, int indent){

}

void PostDecStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out,indent);
  this->myLoc->unparse(out, indent);
  out << "--;\n";
}

void PostIncStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out, indent);
  this->myLoc->unparse(out,indent);
  out << "++;\n";
}

void FromConsoleStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out, indent);
  out << "FROMCONSOLE ";
  this->myLVal->unparse(out, indent);
  out << ";\n";
}

void ToConsoleStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out,indent);
  out << "TOCONSOLE ";
  this->myExp->unparse(out, indent);
  out << ";\n";
}

// IF LPAREN exp RPAREN LCURLY stmtList RCURLY
void IfStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out, indent);
  out << "if (";
  this->myExp->unparse(out,indent);
  out << "){\n";
  std::list<StmtNode *>::iterator it;
  for (it = this->myStmts->begin(); it != this->myStmts->end(); ++it){
    (*it)->unparse(out, indent+1);
  }
  doIndent(out,indent);
  out << "}\n";
}

// IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
void IfElseStmtNode::unparse(std::ostream &out, int indent){
  std::list<StmtNode *>::iterator it;
  std::list<StmtNode *>::iterator it2;
  doIndent(out,indent);
  out << "if (";
  this->myExp->unparse(out, indent);
  out << "){\n";
  for (it = this->myTrueStmts->begin(); it != this->myTrueStmts->end(); ++it){
    doIndent(out,indent);
    (*it)->unparse(out, indent);
  }
  doIndent(out,indent);
  out << "} else {\n";
  for (it = this->myFalseStmts->begin(); it != this->myFalseStmts->end(); ++it){
    doIndent(out,indent);
    (*it)->unparse(out, indent);
  }
  doIndent(out,indent);
  out << "}\n";
}

void WhileStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out,indent);
  out << "while (";
  this->myExp->unparse(out,indent);  
  out << "){\n";
  std::list<StmtNode *>::iterator it;
  for (it = this->myStmts->begin(); it != this->myStmts->end(); ++it){
    (*it)->unparse(out, indent+1);
  }
  doIndent(out, indent);
  out << "}\n";
  
  // WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
}

void ReturnStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out,indent);
  if(this->myExp != nullptr)
  {
    out << "return ";
    this->myExp->unparse(out, indent);
    out << ";\n";
  }
  else{
    out << "return;\n";
  }
}

void IndexNode::unparse(std::ostream &out, int indent){
  this->myIDNode->unparse(out, indent);
  out <<"[";
  this->offsetExpression->hasParens = false;
  this->offsetExpression->unparse(out, indent);
  out <<"]";
}

void DivideNode::unparse(std::ostream &out, int indent){
  if(this->hasParens)
  {
    out << "(";
  }

  this->myLHS->unparse(out, indent);
  out << " / ";
  this->myRHS->unparse(out, indent);

  if(this->hasParens)
  {
      out << ")";
  }

}
void EqualsNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
  this->myLHS->unparse(out,indent);
	out << " == ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens){
    out << ")";}
}

void LessEqNode::unparse(std::ostream &out, int indent){
  if (this->hasParens)
  {
    out << "(";
  }
  this->myLHS->unparse(out,indent);
	out << " <= ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}

void GreaterNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
	this->myLHS->unparse(out,indent);
	out << " > ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}

void CallStmtNode::unparse(std::ostream &out, int indent){
  this->myCallExp->unparse(out,indent);
}

void UnaryExpNode::unparse(std::ostream &out, int indent){
}

void AssignExpNode::unparse(std::ostream &out, int indent){
  this->myLval->unparse(out, indent);
  out << " = ";
  this->myExp->unparse(out, indent);
}
void GreaterEqNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
  this->myLHS->unparse(out, indent);
  out << " >= ";
  this->myRHS->unparse(out, indent);
  if(this->hasParens)
  {
    out << ")";
  }
}
void NotEqualsNode::unparse(std::ostream &out, int indent){
  if (this->hasParens)
  {
    out << "(";
  }
  this->myLHS->unparse(out, indent);
  out << " != ";
  this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}
void AssignStmtNode::unparse(std::ostream &out, int indent){
  doIndent(out,indent);
  this->myAssignExp->myLval->unparse(out, indent);
  out << " = ";
  this->myAssignExp->myExp->unparse(out, indent);
  out << ";\n";
}
void OrNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
  this->myLHS->unparse(out, indent);
  out << " || ";
  this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}
void AndNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
  this->myLHS->unparse(out, indent);
  out << " && ";
  this->myRHS->unparse(out, indent);
  if(this->hasParens)
  {
      out << ")";
  }
}
void NegNode::unparse(std::ostream &out, int indent){
  out << " - ";
  this->myExp->unparse(out,indent);
}
void NotNode::unparse(std::ostream &out, int indent){
  out << "!";
  this->myExp->unparse(out,indent);
}
void DeclNode::unparse(std::ostream &out, int indent){

}

void LessNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
	this->myLHS->unparse(out,indent);
	out << " < ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}
void PlusNode::unparse(std::ostream &out, int indent){
  if(this->hasParens){  
    out << "(";}
	this->myLHS->unparse(out,indent);
	out << " + ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}
void MinusNode::unparse(std::ostream &out, int indent){
  if (this->hasParens)
  {
    out << "(";
  }
  this->myLHS->unparse(out,indent);
	out << " - ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}
void TimesNode::unparse(std::ostream &out, int indent){
  if (this->hasParens)
  {
    out << "(";
  }
  this->myLHS->unparse(out,indent);
	out << " * ";
	this->myRHS->unparse(out,indent);
  if(this->hasParens)
  {
      out << ")";
  }
}

// void LValNode::unparse(std::ostream &out, int indent){
  
// }

void DerefNode::unparse(std::ostream &out, int indent){
  out <<"@";
  this->myIDNode->unparse(out, indent);
}

void RefNode::unparse(std::ostream &out, int indent){
  out <<"^";
  this->myIDNode->unparse(out, indent);
}

} // End namespace holeyc
