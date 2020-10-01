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
	throw new ToDoError("[DELETE ME] I'm a varDecl"
		" you should add the information from my"	
		" subtree to the symbolTable as a new"	
		" entry in the current scope table"
	);

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
  throw new ToDoError("[DELETE ME] I'm an fnDecl."
		" you should add and make current a new"	
		" scope table for my body"
	);

  this->myID;
  this->myRetType;
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
    // ReturnStmtNode();
    // CallStmtNode();
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

// CLB: Do not ned this, we will expect varDecl to deal with it's ID but not actually call nameanalysis on it 
// bool IDNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
bool RefNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  this->getMyID();
  return nameAnalysisOk;
}
bool DerefNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  this->getMyID(); 
  return nameAnalysisOk;
}
bool IndexNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true;
  this->getMyID(); 
  return nameAnalysisOk;
}

/* CLB: Will likely never hit an actual TypeNode since varDeclNode's nameanalysis() 
 * doesn't call nameanalysis() on it's TypeNode*
 */
// bool TypeNode::nameAnalysis(SymbolTable * symTab){ throw new ToDoError("This function should have been overriden in the subclass!"); }
// bool CharTypeNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
// bool VoidTypeNode::nameAnalysis(SymbolTable *symTab) { bool nameAnalysisOk = true; }
// bool IntTypeNode::nameAnalysis(SymbolTable *symTab) { bool nameAnalysisOk = true; }
// bool BoolTypeNode::nameAnalysis(SymbolTable *symTab) { bool nameAnalysisOk = true; }

bool StmtNode::nameAnalysis(SymbolTable * symTab){
  bool nameAnalysisOk = true;
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
  return nameAnalysisOk;
 }
bool DeclNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool AssignStmtNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = this->getMyExp()->nameAnalysis(symTab);
  return nameAnalysisOk;
}
bool FromConsoleStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool ToConsoleStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool PostIncStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool IfStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool WhileStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool CallStmtNode::nameAnalysis(SymbolTable *symTab) { bool nameAnalysisOk = true; return nameAnalysisOk; }

bool BinaryExpNode::nameAnalysis(SymbolTable *symTab) {
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
 }
bool CallExpNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool PlusNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool MinusNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool TimesNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool DivideNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool AndNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool OrNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool EqualsNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool NotEqualsNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool LessNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool LessEqNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool GreaterNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool GreaterEqNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool UnaryExpNode::nameAnalysis(SymbolTable * symTab){
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
 }
bool NegNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }
bool NotNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; return nameAnalysisOk; }

bool AssignExpNode::nameAnalysis(SymbolTable * symTab){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = this->getMyDst()->nameAnalysis(symTab) && this->getMySrc()->nameAnalysis(symTab);
  return nameAnalysisOk;
}

/* CLB: Will likely never hit an actual value node since our assignmentNode's nameanalysis() 
 * won't call nameanalysis() on it's valueNodes.
 */
// bool IntLitNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
// bool StrLitNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
// bool CharLitNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
// bool NullPtrNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
// bool TrueNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }
// bool FalseNode::nameAnalysis(SymbolTable * symTab){ bool nameAnalysisOk = true; }

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