#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"

namespace holeyc{

//TODO here is a subset of the nodes needed to do nameAnalysis, 
// you should add the rest to allow for a complete treatment
// of any AST

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
	// throw new ToDoError("[DELETE ME] I'm a varDecl"
	// 	" you should add the information from my"	
	// 	" subtree to the symbolTable as a new"	
	// 	" entry in the current scope table"
	// );

  VarSymbol * vS = new VarSymbol(this);
  symTab->currentScope()->addSymbol(vS);
  

	// Create a new entry in the symbol table inside the last scope.
	TypeNode * type_node = this->getTypeNode();
	return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
  // Check the return type
  // Check the ID make sure it doesn't clash
  // Check the formals, make sure they are kosher
  // Descend into the statements to check them 
  // throw new ToDoError("[DELETE ME] I'm an fnDecl."
	// 	" you should add and make current a new"	
	// 	" scope table for my body"
    
	// );

  FnSymbol * fS = new FnSymbol(this);
  symTab->currentScope()->addSymbol(fS);

  // Create a new scope table and append it to the end of the symbol table.
  ScopeTable * scope = new ScopeTable();
  symTab->addScope(scope);
  bool res = true;
  for (auto formal : *myFormals){ 
    // formal = FormalDeclNode : public VarDeclNode
    res = formal->nameAnalysis(symTab) && res;
  }

  for (auto body : *myBody){
    // Function Decls <- add to scope (call nameAnalysis)
    // Assignments <- check SymbolTable for IDs
    // Var Decls <- add to scope (call nameAnalysis)
    res = body->nameAnalysis(symTab) && res;
    // VarDeclNode(); DONE
    // AssignStmtNode(); 
    // PostDecStmtNode();
    // PostIncStmtNode();
    // FromConsoleStmtNode();
    // ToConsoleStmtNode();
    // IfStmtNode();
    // IfElseStmtNode();
    // WhileStmtNode();
    // ReturnStmtNode(); 
    // CallStmtNode(); DONE 
  }
  return nameAnalysisOk;
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab){
  bool nameAnalysisOk = true;
  throw new ToDoError("[DELETE ME] I'm a varDecl"
                      " you should add the information from my"
                      " subtree to the symbolTable as a new"
                      " entry in the current scope table");

  VarSymbol * vS = new VarSymbol(this);
  symTab->currentScope()->addSymbol(vS);

  // Create a new entry in the symbol table inside the last scope.
  TypeNode * type_node = this->getTypeNode();
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

// CLB: Do not ned this, we will expect varDecl to deal with it's ID but not actually call nameanalysis on it 
bool IDNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  // nameAnalysisOk = symTab->isInScopeChain(this);
  if(nameAnalysisOk){
    // print ID and ID's type.
    std::string toPrint = name; 
    toPrint += " " + this->mySymbol->myTypeToS() + "\n";
    std::cout << toPrint;
  }
  else{
    // ERROR: Undeclared identifier.
    std::cout << "L,C: Undeclared Identifier\n";
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
  nameAnalysisOk = myExp->nameAnalysis(symTab);
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
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool MinusNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool TimesNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool DivideNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool AndNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool OrNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool EqualsNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool NotEqualsNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool LessNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool LessEqNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool GreaterNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
  return nameAnalysisOk; 
}
bool GreaterEqNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab) && myExp2->nameAnalysis(symTab);
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
  nameAnalysisOk = myExp->nameAnalysis(symTab);
  return nameAnalysisOk; 
}

bool AssignExpNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = myDst->nameAnalysis(symTab) && mySrc->nameAnalysis(symTab);
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