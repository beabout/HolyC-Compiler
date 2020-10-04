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
  if(res){
    this->unparse(std::cout, 0);
  }
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
  // ERROR: Invalid type in declaration.
  if(this->getTypeNode()->getMyString() == "void"){
    std::cerr << this->line() << "," << this->col() << ": Invalid type in declaration\n";
    nameAnalysisOk = false; 
  }
  // ERROR: Multiply declared identifier
  if(symTab->lookUp(this->ID()->getName())) {
    std::cerr << this->line() << "," << this->col() << ": Multiply declared identifier\n";
    nameAnalysisOk = false; 
  } else {
    VarSymbol * vS = new VarSymbol(this);
    this->myID->SetSymbol(vS);
    symTab->currentScope()->addSymbol(this->myID->getName(), vS);
    // Create a new entry in the symbol table inside the last scope.
    TypeNode * type_node = this->getTypeNode();
  }
  return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
  if(symTab->lookUp(this->ID()->getName())){
    std::cerr << this->line() << "," << this->col() << ": Multiply declared identifier\n";
  } else {  
    FnSymbol * fS = new FnSymbol(this);
    symTab->currentScope()->addSymbol(this->myID->getName(), fS);
  }  
  myRetType->nameAnalysis(symTab);
  // Create a new scope table and append it to the end of the symbol table.
  ScopeTable * scope = new ScopeTable();
  symTab->addScope(scope);
  int formal_size = myFormals->size();
  int count = 0;
  for (auto formal : *myFormals){ 
    // formal = FormalDeclNode : public VarDeclNode
    nameAnalysisOk = formal->nameAnalysis(symTab) && nameAnalysisOk;
    count++;
    if(count != formal_size)
    {
    }
  }
  for (auto body : *myBody){
    // Function Decls <- add to scope (call nameAnalysis)
    // Assignments <- check SymbolTable for IDs
    // Var Decls <- add to scope (call nameAnalysis)    
    nameAnalysisOk = body->nameAnalysis(symTab) && nameAnalysisOk;
  }  
  symTab->dropScope();
  return nameAnalysisOk;
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab){
  bool nameAnalysisOk = true;
  // ERROR: Invalid type in declaration.
  if (this->getTypeNode()->getMyString() == "Void"){
    std::cerr << this->line() << "," << this->col() << ": Invalid type in declaration\n";
    nameAnalysisOk = false;
  }
  // ERROR: Multiply declared identifier
  if (symTab->lookUp(this->ID()->getName())){
    std::cerr << this->line() << "," << this->col() << ": Multiply declared identifier\n";
    nameAnalysisOk = false;
  }
  else{
    VarSymbol * vS = new VarSymbol(this);
    this->ID()->SetSymbol(vS);
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
  if(nameAnalysisOk){
    // print ID and ID's type.
    std::string toPrint = getName(); 
    if(mySymbol != nullptr){
      toPrint += "(" + mySymbol->myTypeToS() + ")";
    }
  }
  else{
    // ERROR: Undeclared identifier.
    std::cerr << this->line() << "," << this->col() << ": Undeclared Identifier\n";
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
  if(isPtr){
  } else {
  }
  return nameAnalysisOk; 
}
bool VoidTypeNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true; 
  return nameAnalysisOk; 
}
bool IntTypeNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true; 
  if(isPtr){
  }else{
  }
  return nameAnalysisOk; 
}
bool BoolTypeNode::nameAnalysis(SymbolTable *symTab) { 
  bool nameAnalysisOk = true; 
  if(isPtr){
  } else {
  }
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
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
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
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool MinusNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk =  myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool TimesNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool DivideNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool AndNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool OrNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool EqualsNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool NotEqualsNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool LessNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool LessEqNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool GreaterNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool GreaterEqNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;  
  nameAnalysisOk = myExp1->nameAnalysis(symTab);
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool UnaryExpNode::nameAnalysis(SymbolTable * symTab){
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
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
  nameAnalysisOk = nameAnalysisOk && mySrc->nameAnalysis(symTab);
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

// int fn(bool a, int c){
//   bool a;
//   int c;
//   int f; 
//   c = 3; 
//   f = c; 
//   a = false; 
// }

// Scope 
//   - add bool a, 
//   - add int c, 
//   - add int f, 
//   - check for variables as needed. 
//   - add other decls if needed. 
//   - loop through last two steps.