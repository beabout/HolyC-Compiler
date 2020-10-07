#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"

namespace holeyc{

//TODO here is a subset of the nodes needed to do nameAnalysis, 
// you should add the rest to allow for a complete treatment
// of any AST

static void doIndent(int indent){
	for (int k = 0 ; k < indent; k++){ std::cout << "\t"; }
}

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab){
	// Make global scope table.
	
  ScopeTable * globalScope = new ScopeTable();
  symTab->addScope(globalScope);
  bool res = true;
	for (auto global : *myGlobals){ // Iterate through varDecls and fnDecls 
		res = global->nameAnalysis(symTab) && res;
	}
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
  // ERROR: Invalid type in declaration.
  if(this->getTypeNode()->getMyString() == "void"){
    std::cerr <<  "FATAL [" << this->getTypeNode()->line() << "," << this->getTypeNode()->col() << "]: Invalid type in declaration\n";
    nameAnalysisOk = false; 
  }
  // ERROR: Multiply declared identifier
  if(symTab->lookUp(this->ID()->getName())) {
    std::cerr <<  "FATAL [" << this->ID()->line() << "," << this->ID()->col() << "]: Multiply declared identifier\n";
    nameAnalysisOk = false; 
  } else {
    VarSymbol * vS = new VarSymbol(this);
    symTab->currentScope()->addSymbol(this->myID->getName(), vS);
    // Create a new entry in the symbol table inside the last scope.
    TypeNode * type_node = this->getTypeNode();
  }
  return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
  if(symTab->lookUp(this->ID()->getName())){
    std::cerr <<  "FATAL [" << this->ID()->line() << "," << this->ID()->col() << "]: Multiply declared identifier\n";
    nameAnalysisOk = false;
  } else {  
    FnSymbol * fS = new FnSymbol(this);
    symTab->currentScope()->addSymbol(this->myID->getName(), fS);
  }  
  myRetType->nameAnalysis(symTab);
  // Create a new scope table and append it to the end of the symbol table.
  ScopeTable * scope = new ScopeTable();
  symTab->addScope(scope);
  for (auto formal : *myFormals){ 
    nameAnalysisOk = formal->nameAnalysis(symTab) && nameAnalysisOk;
  }
  for (auto body : *myBody){
    nameAnalysisOk = body->nameAnalysis(symTab) && nameAnalysisOk;
  }  
  symTab->dropScope();
  return nameAnalysisOk;
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab){
  bool nameAnalysisOk = true;
  // ERROR: Invalid type in declaration.
  if (this->getTypeNode()->getMyString() == "void"){
    std::cerr <<  "FATAL [" << this->getTypeNode()->line() << "," << this->getTypeNode()->col() << "]: Invalid type in declaration\n";
    nameAnalysisOk = false;
  }
  // ERROR: Multiply declared identifier
  if (symTab->lookUp(this->ID()->getName())){
    std::cerr <<  "FATAL [" << this->ID()->line() << "," << this->ID()->col() << "]: Multiply declared identifier\n";
    nameAnalysisOk = false;
  }
  else{
    VarSymbol * vS = new VarSymbol(this);
    symTab->currentScope()->addSymbol(this->ID()->getName(), vS);
    // Create a new entry in the symbol table inside the last scope.
    TypeNode *type_node = this->getTypeNode();
    this->getTypeNode()->nameAnalysis(symTab);
  }
  return nameAnalysisOk;
}

// bool ExpNode::nameAnalysis(SymbolTable * symTab){
//   throw new ToDoError("This function should have"
//                       "been overriden in the subclass!");
// }

// bool LValNode::nameAnalysis(SymbolTable * symTab){
//   throw new ToDoError("This function should have"
//                       "been overriden in the subclass!");
// }

// CLB: Do not need this, we will expect varDecl to deal with it's ID but not actually call nameanalysis on it 
bool IDNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = symTab->isInScopeChain(this);
  if(!nameAnalysisOk){
    // ERROR: Undeclared identifier.
    std::cerr << "FATAL [" << this->line() << "," << this->col() << "]: Undeclared Identifier\n";
    nameAnalysisOk = false;
  }
  return nameAnalysisOk; 
}
bool RefNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myID->nameAnalysis(symTab);
  return nameAnalysisOk;
}
bool DerefNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myID->nameAnalysis(symTab); 
  return nameAnalysisOk;
}
bool IndexNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myBase->nameAnalysis(symTab);
  nameAnalysisOk = myOffset->nameAnalysis(symTab);
  return nameAnalysisOk;
}

/* CLB: Will likely never hit an actual TypeNode since varDeclNode's nameanalysis() 
 * doesn't call nameanalysis() on it's TypeNode*
 */
// bool TypeNode::nameAnalysis(SymbolTable * symTab){ 
//   bool nameAnalysisOk = true; 
//   throw new ToDoError("This function should have been overriden in the subclass!"); return nameAnalysisOk; 
// }
bool CharTypeNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  return nameAnalysisOk; 
}
bool VoidTypeNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true; 
  return nameAnalysisOk; 
}
bool IntTypeNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true; 
  return nameAnalysisOk; 
}
bool BoolTypeNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true; 
  return nameAnalysisOk; 
}

// bool StmtNode::nameAnalysis(SymbolTable * symTab){
//   bool nameAnalysisOk = true;
//   throw new ToDoError("This function should have"
//                       "been overriden in the subclass!");
//   return nameAnalysisOk;
// }
// bool DeclNode::nameAnalysis(SymbolTable * symTab){ 
//   bool nameAnalysisOk = true; 
//   return nameAnalysisOk; 
// }


bool AssignStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  // Step One: check if my ID is valid.
  // name analysis for an IDNODE will check if it exists in scope.
  nameAnalysisOk = myExp->nameAnalysis(symTab);
  return nameAnalysisOk;
}
bool FromConsoleStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myDst->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool ToConsoleStmtNode::nameAnalysis(SymbolTable * symTab){
  bool nameAnalysisOk = true;
  nameAnalysisOk = mySrc->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myLVal->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool PostIncStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myLVal->nameAnalysis(symTab);
  return nameAnalysisOk;
}
bool IfStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myCond->nameAnalysis(symTab);
  for (auto stmt : *myBody){    
    nameAnalysisOk = stmt->nameAnalysis(symTab);
  }  
  return nameAnalysisOk; 
}
bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myCond->nameAnalysis(symTab);
  for (auto stmt : *myBodyTrue){    
    nameAnalysisOk = stmt->nameAnalysis(symTab);
  }  
  for (auto stmt : *myBodyFalse){    
    nameAnalysisOk = stmt->nameAnalysis(symTab);
  }  
  return nameAnalysisOk;
}
bool WhileStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myCond->nameAnalysis(symTab);
  for (auto stmt : *myBody){
    nameAnalysisOk = stmt->nameAnalysis(symTab);
  }  
  return nameAnalysisOk;
}
bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  if(myExp){
    nameAnalysisOk = myExp->nameAnalysis(symTab);
  }
  return nameAnalysisOk; 
}
bool CallStmtNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myCallExp->nameAnalysis(symTab);
  return nameAnalysisOk;
}
bool BinaryExpNode::nameAnalysis(SymbolTable *symTab) {
  bool nameAnalysisOk = true;
  return nameAnalysisOk;
}
bool CallExpNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myID->nameAnalysis(symTab);
  for (auto arg : *myArgs){
    nameAnalysisOk = arg->nameAnalysis(symTab);
  }
  return nameAnalysisOk;
}
bool PlusNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool MinusNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk =  myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool TimesNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool DivideNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool AndNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool OrNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool EqualsNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool NotEqualsNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool LessNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool LessEqNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool GreaterNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool GreaterEqNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = myExp2->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk; 
}
bool UnaryExpNode::nameAnalysis(SymbolTable * symTab){
  bool nameAnalysisOk = true;
  return nameAnalysisOk;
}
bool NegNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;   
  nameAnalysisOk = myExp->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool NotNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
	//myExp->unparseNested(out); 
  nameAnalysisOk = myExp->nameAnalysis(symTab);
  return nameAnalysisOk; 
}

bool AssignExpNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = myDst->nameAnalysis(symTab);
  nameAnalysisOk = mySrc->nameAnalysis(symTab) && nameAnalysisOk;
  return nameAnalysisOk;
}

/* CLB: Will likely never hit an actual value node since our assignmentNode's nameanalysis() 
 * won't call nameanalysis() on it's valueNodes.
 */
bool IntLitNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;   
  return nameAnalysisOk; 
}
bool StrLitNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  return nameAnalysisOk; 
}
bool CharLitNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;   
  return nameAnalysisOk; 
}
bool NullPtrNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;   
  return nameAnalysisOk; 
}
bool TrueNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;   
  return nameAnalysisOk; 
}
bool FalseNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;   
  return nameAnalysisOk; 
}

}