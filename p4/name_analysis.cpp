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

bool ASTNode::nameAnalysis(SymbolTable * symTab, int indent){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab, int indent){
	// Make global scope table.
	
  ScopeTable * globalScope = new ScopeTable();
  symTab->addScope(globalScope);
  bool res = true;
	for (auto global : *myGlobals){ // Iterate through varDecls and fnDecls 
		res = global->nameAnalysis(symTab,indent) && res;
	}
  // call unparse here
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab, int indent){
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
    //doIndent(indent);
    myType->nameAnalysis(symTab,indent);
    std::cout << myID->getName() << ";\n";
  }
  return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab, int indent){
	bool nameAnalysisOk = true;
  if(symTab->lookUp(this->ID()->getName())){
    std::cerr << this->line() << "," << this->col() << ": Multiply declared identifier\n";
  } else {  
    FnSymbol * fS = new FnSymbol(this);
    symTab->currentScope()->addSymbol(this->myID->getName(), fS);
  }
  doIndent(indent);
  myRetType->nameAnalysis(symTab,indent);
  std::cout << this->ID()->getName() << "(";
  // Create a new scope table and append it to the end of the symbol table.
  ScopeTable * scope = new ScopeTable();
  symTab->addScope(scope);
  int formal_size = myFormals->size();
  int count = 0;
  for (auto formal : *myFormals){ 
    // formal = FormalDeclNode : public VarDeclNode
    nameAnalysisOk = formal->nameAnalysis(symTab,indent) && nameAnalysisOk;
    count++;
    if(count != formal_size)
    {
      std::cout << ", ";
    }
  }
  std::cout << "){\n";
  for (auto body : *myBody){
    // Function Decls <- add to scope (call nameAnalysis)
    // Assignments <- check SymbolTable for IDs
    // Var Decls <- add to scope (call nameAnalysis)
    doIndent(indent+1);
    nameAnalysisOk = body->nameAnalysis(symTab,indent+1) && nameAnalysisOk;
  }
  doIndent(indent);
  std::cout << "}\n";
  symTab->dropScope();
  return nameAnalysisOk;
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab, int indent){
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
    this->getTypeNode()->nameAnalysis(symTab,indent);
    std::cout << this->ID()->getName();
  }
  return nameAnalysisOk;
}

// bool ExpNode::nameAnalysis(SymbolTable * symTab, int indent){
//   throw new ToDoError("This function should have"
//                       "been overriden in the subclass!");
// }

// bool LValNode::nameAnalysis(SymbolTable * symTab, int indent){
//   throw new ToDoError("This function should have"
//                       "been overriden in the subclass!");
// }

// CLB: Do not need this, we will expect varDecl to deal with it's ID but not actually call nameanalysis on it 
bool IDNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = symTab->isInScopeChain(this);
  if(nameAnalysisOk){
    // print ID and ID's type.
    std::string toPrint = getName(); 
    if(mySymbol != nullptr){
      toPrint += "(" + mySymbol->myTypeToS() + ")";
    }
    std::cout << toPrint;
  }
  else{
    // ERROR: Undeclared identifier.
    std::cerr << this->line() << "," << this->col() << ": Undeclared Identifier\n";
  }
  return nameAnalysisOk; 
}
bool RefNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "^";
  nameAnalysisOk = myID->nameAnalysis(symTab,indent);
  return nameAnalysisOk;
}
bool DerefNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "@";
  nameAnalysisOk = myID->nameAnalysis(symTab,indent); 
  return nameAnalysisOk;
}
bool IndexNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myBase->nameAnalysis(symTab,indent);
  std::cout << "[";
  nameAnalysisOk = myOffset->nameAnalysis(symTab,indent);
  std::cout << "]";
  return nameAnalysisOk;
}

/* CLB: Will likely never hit an actual TypeNode since varDeclNode's nameanalysis() 
 * doesn't call nameanalysis() on it's TypeNode*
 */
// bool TypeNode::nameAnalysis(SymbolTable * symTab, int indent){ 
//   bool nameAnalysisOk = true; 
//   throw new ToDoError("This function should have been overriden in the subclass!"); return nameAnalysisOk; 
// }
bool CharTypeNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  if(isPtr){
    std::cout << "charptr ";
  } else {
  std::cout << "char ";
  }
  return nameAnalysisOk; 
}
bool VoidTypeNode::nameAnalysis(SymbolTable *symTab, int indent) { 
  bool nameAnalysisOk = true; 
  std::cout << "void ";
  return nameAnalysisOk; 
}
bool IntTypeNode::nameAnalysis(SymbolTable *symTab, int indent) { 
  bool nameAnalysisOk = true; 
  if(isPtr){
    std::cout << "intptr ";
  }else{
    std::cout << "int ";
  }
  return nameAnalysisOk; 
}
bool BoolTypeNode::nameAnalysis(SymbolTable *symTab, int indent) { 
  bool nameAnalysisOk = true; 
  if(isPtr){
    std::cout << "boolptr ";
  } else {
    std::cout << "bool ";
  }
  return nameAnalysisOk; 
}

// bool StmtNode::nameAnalysis(SymbolTable * symTab, int indent){
//   bool nameAnalysisOk = true;
//   throw new ToDoError("This function should have"
//                       "been overriden in the subclass!");
//   return nameAnalysisOk;
// }
// bool DeclNode::nameAnalysis(SymbolTable * symTab, int indent){ 
//   bool nameAnalysisOk = true; 
//   return nameAnalysisOk; 
// }


bool AssignStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  // Step One: check if my ID is valid.
  // name analysis for an IDNODE will check if it exists in scope.
  nameAnalysisOk = myExp->nameAnalysis(symTab,indent);
  return nameAnalysisOk;
}
bool FromConsoleStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "FROMCONSOLE ";
  nameAnalysisOk = myDst->nameAnalysis(symTab,indent);
  std::cout << ";\n";
  return nameAnalysisOk; 
}
bool ToConsoleStmtNode::nameAnalysis(SymbolTable * symTab, int indent){
  bool nameAnalysisOk = true;
  std::cout << "TOCONSOLE ";
  nameAnalysisOk = mySrc->nameAnalysis(symTab,indent);
  std::cout << ";\n";
  return nameAnalysisOk; 
}
bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myLVal->nameAnalysis(symTab,indent);
  std::cout << "--;\n";
  return nameAnalysisOk; 
}
bool PostIncStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myLVal->nameAnalysis(symTab,indent);
  std::cout << "++;\n";
  return nameAnalysisOk;
}
bool IfStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "if(";
  nameAnalysisOk = myCond->nameAnalysis(symTab,indent);
  std::cout << "){\n";
  for (auto stmt : *myBody){
    doIndent(indent+1);
    nameAnalysisOk = stmt->nameAnalysis(symTab,indent+1);
  }
  doIndent(indent);
  std::cout << "}\n";
  return nameAnalysisOk; 
}
bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "if(";
  nameAnalysisOk = myCond->nameAnalysis(symTab,indent+1);
  std::cout << "){\n";
  for (auto stmt : *myBodyTrue){
    doIndent(indent+1);
    nameAnalysisOk = stmt->nameAnalysis(symTab,indent+1);
  }
  doIndent(indent);
  std::cout << "}else{\n";
  for (auto stmt : *myBodyFalse){
    doIndent(indent+1);
    nameAnalysisOk = stmt->nameAnalysis(symTab,indent+1);
  }
  doIndent(indent);
  std::cout << "}";
  return nameAnalysisOk;
}
bool WhileStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "while(";
  nameAnalysisOk = myCond->nameAnalysis(symTab,indent+1);
  std::cout << "){\n";
  for (auto stmt : *myBody){
    doIndent(indent + 1);
    nameAnalysisOk = stmt->nameAnalysis(symTab,indent+1);
  }
  doIndent(indent);
  std::cout << "}\n";
  return nameAnalysisOk;
}
bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  std::cout << "return ";
  if(myExp){
    nameAnalysisOk = myExp->nameAnalysis(symTab,indent);
  }
  std::cout << ";\n";
  return nameAnalysisOk; 
}
bool CallStmtNode::nameAnalysis(SymbolTable *symTab, int indent) { 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myCallExp->nameAnalysis(symTab,indent+1);
  return nameAnalysisOk;
}
bool BinaryExpNode::nameAnalysis(SymbolTable *symTab, int indent) {
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
}
bool CallExpNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myID->nameAnalysis(symTab,indent);
  std::cout << "(";
  for (auto arg : *myArgs){
    nameAnalysisOk = arg->nameAnalysis(symTab,indent);
  }
  std::cout << ")";
  std::cout << ";\n";
  return nameAnalysisOk;
}
bool PlusNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " + ";
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool MinusNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  nameAnalysisOk =  myExp1->nameAnalysis(symTab,indent);
	std::cout << " - ";
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool TimesNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " * ";
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool DivideNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " / ";
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool AndNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
  std::cout << " && ";
  nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool OrNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " || ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool EqualsNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " == ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool NotEqualsNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " != ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool LessNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " < ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool LessEqNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " <= ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool GreaterNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " > ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool GreaterEqNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  // doIndent(indent);
  nameAnalysisOk = myExp1->nameAnalysis(symTab,indent);
	std::cout << " >= ";
   nameAnalysisOk = nameAnalysisOk && myExp2->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool UnaryExpNode::nameAnalysis(SymbolTable * symTab, int indent){
  throw new ToDoError("This function should have"
                      "been overriden in the subclass!");
}
bool NegNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  // doIndent(indent);
	std::cout << "-";
  nameAnalysisOk = myExp->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}
bool NotNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  // doIndent(indent);
	std::cout << "!";
	//myExp->unparseNested(out); 
  nameAnalysisOk = myExp->nameAnalysis(symTab,indent);
  return nameAnalysisOk; 
}

bool AssignExpNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  nameAnalysisOk = myDst->nameAnalysis(symTab,indent);
  std::cout << " = ";
  nameAnalysisOk = nameAnalysisOk && mySrc->nameAnalysis(symTab,indent);
  std::cout << ";\n";
  return nameAnalysisOk;
}

/* CLB: Will likely never hit an actual value node since our assignmentNode's nameanalysis() 
 * won't call nameanalysis() on it's valueNodes.
 */
bool IntLitNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  //doIndent(indent);
	std::cout << myNum;
  return nameAnalysisOk; 
}
bool StrLitNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true;
  //doIndent(indent);
	std::cout << myStr;
  return nameAnalysisOk; 
}
bool CharLitNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  //doIndent(indent);
	if (myVal == '\n'){
		std::cout << "'\\n";
	} else if (myVal == '\t'){
		std::cout << "'\\t";
	} else {
		std::cout << "'" << myVal;
	}
  return nameAnalysisOk; 
}
bool NullPtrNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  //doIndent(indent);
  std::cout << "nullptr";
  return nameAnalysisOk; 
}
bool TrueNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  //doIndent(indent);
  std::cout << "true";
  return nameAnalysisOk; 
}
bool FalseNode::nameAnalysis(SymbolTable * symTab, int indent){ 
  bool nameAnalysisOk = true; 
  //doIndent(indent);
  std::cout << "false";
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