#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

// arithmetic operand (opd) : When the operand would never work for the expression calling it (ex. applying a function name to an equality f == bool)
// fn() == void 

// arithmetic operator (opr) : When the operands are valid types (int, bool) but the types are different. 
// 3 == 3 OK
// Bool == Bool OK
// 3 == Bool Operator error



namespace holeyc{

TypeAnalysis * TypeAnalysis::build(NameAnalysis * nameAnalysis){
	//To emphasize that type analysis depends on name analysis
	// being complete, a name analysis must be supplied for
	// type analysis to be performed.
	TypeAnalysis * typeAnalysis = new TypeAnalysis();
	auto ast = nameAnalysis->ast;
	typeAnalysis->ast = ast;

	ast->typeAnalysis(typeAnalysis);
	if (typeAnalysis->hasError){
		return nullptr;
	}

	return typeAnalysis;

}

void ProgramNode::typeAnalysis(TypeAnalysis * ta){

	//pass the TypeAnalysis down throughout
	// the entire tree, getting the types for
	// each element in turn and adding them
	// to the ta object's hashMap
  for (auto global : *myGlobals){
    global->typeAnalysis(ta);
  }
	//The type of the program node will never
	// be needed. We can just set it to VOID
	//(Alternatively, we could make our type
	// be error if the DeclListNode is an error)
  ta->nodeType(this, BasicType::produce(VOID));

}

void FnDeclNode::typeAnalysis(TypeAnalysis *ta){
  //HINT: you might want to change the signature for
	// typeAnalysis on FnBodyNode to take a second
	// argument which is the type of the current
	// function. This will help you to know at a
	// return statement whether the return type matches
	// the current function
	DataType* return_type = this->getRetTypeNode()->getType();

  std::list<const DataType*>* temp_list = new std::list<const DataType*>();
  for(auto formal : *myFormals){
    // push datatype of formal to list.
    DataType* data_type = formal->getTypeNode()->getType();
    const DataType* const_data_type = const_cast<DataType*>(data_type);
    temp_list->push_back(const_data_type);
  }

	const std::list<const DataType*>* formal_type_list = const_cast<std::list<const DataType*>*>(temp_list);
  FnType* my_fn_type = new FnType(formal_type_list, return_type);
	ta->setCurrentFnType(my_fn_type);
  ta->nodeType(this, my_fn_type);

  for (auto stmt : *myBody){
    // stmt could be a return stmt. Must know Fn return type.
    stmt->typeAnalysis(ta);
    // if we have a ReturnStmtNode, it should return the fn return type.
    // Access via getCurrentFnType() inside each StmtNode
  }
}

void StmtNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Implement me in the subclass");
}

void AssignStmtNode::typeAnalysis(TypeAnalysis * ta){
	myExp->typeAnalysis(ta);

	//It can be a bit of a pain to write
	// "const DataType *" everywhere, so here
	// the use of auto is used instead to tell the
	// compiler to figure out what the subType variable
	// should be
	auto subType = ta->nodeType(myExp);

	// As error returns null if subType is NOT an error type
	// otherwise, it returns the subType itself
	if (subType->asError()){
		ta->nodeType(this, subType);
  } else {
    ta->nodeType(this, BasicType::produce(VOID));
	}
}

void ExpNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void AssignExpNode::typeAnalysis(TypeAnalysis * ta){
	//Do typeAnalysis on the subexpressions
	myDst->typeAnalysis(ta);
	mySrc->typeAnalysis(ta);

	const DataType * lhsType = ta->nodeType(myDst);
	const DataType * rhsType = ta->nodeType(mySrc);

  if(rhsType->asError()){
    ta->nodeType(this, ErrorType::produce());
    return;
  } else {
    if (lhsType == rhsType){
      if (lhsType->asFn()){
        // Throw error, Invalid assignment operand
        ta->badAssignOpd(myDst->line(), myDst->col());
        ta->nodeType(this, ErrorType::produce());
      } else {
        ta->nodeType(this, lhsType);
      }
      return;
    } else {
      //Some functions are already defined for you to
      // report type errors. Note that these functions
      // also tell the typeAnalysis object that the
      // analysis has failed, meaning that main.cpp
      // will print "Type check failed" at the end
      ta->badAssignOpr(this->line(), this->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
  }
}

void DeclNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis * ta){
	// VarDecls always pass type analysis, since they
	// are never used in an expression. You may choose
	// to type them void (like this), as discussed in class

  ta->nodeType(this->myID, this->myType->getType());
	ta->nodeType(this, BasicType::produce(VOID));
}

void FormalDeclNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this->ID(), this->getTypeNode()->getType());
  ta->nodeType(this, BasicType::produce(VOID));
}

void IDNode::typeAnalysis(TypeAnalysis * ta){
	// IDs never fail type analysis and always
	// yield the type of their symbol (which
	// depends on their definition)

  // This will always be called in a use, not a definition
  ta->nodeType(this, this->getSymbol()->getDataType());
}

void LValNode::typeAnalysis(TypeAnalysis * ta){
  // override in subclass
}

void FromConsoleStmtNode::typeAnalysis(TypeAnalysis *ta){
  myDst->typeAnalysis(ta);
  if (ta->nodeType(myDst)->asFn()){
    // ERROR: trying to read a function from console.
    ta->readFn(myDst->line(), myDst->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  else
  {
    ta->nodeType(this, BasicType::produce(VOID)); // <- I suppose this is void..
    return;
  }
}

void ToConsoleStmtNode::typeAnalysis(TypeAnalysis *ta){
  mySrc->typeAnalysis(ta);
  const DataType * my_src_type = ta->nodeType(mySrc);
  if (my_src_type->asFn()){
    if (my_src_type->asFn()->getReturnType() == BasicType::produce(VOID)){
      // attempt to write void 
      ta->badWriteVoid(mySrc->line(), mySrc->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
    else {
      // ERROR: attempting to output a function
      ta->badWriteVoid(mySrc->line(), mySrc->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
  } else {
    ta->nodeType(this, BasicType::produce(VOID)); // <- I suppose this is void..
    return;
  }
}

void IfStmtNode::typeAnalysis(TypeAnalysis *ta){
  myCond->typeAnalysis(ta);
  if(ta->nodeType(myCond) != BasicType::produce(BOOL) && ta->nodeType(myCond) != ErrorType::produce()){
    ta->badIfCond(myCond->line(), myCond->col());
    ta->nodeType(this, ErrorType::produce());
  }
  else{
    for(auto stmt : *myBody){
      stmt->typeAnalysis(ta);
    }
  }
  ta->nodeType(this, BasicType::produce(VOID));
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis *ta){
  myCond->typeAnalysis(ta);
  if(ta->nodeType(myCond) != BasicType::produce(BOOL) && ta->nodeType(myCond) != ErrorType::produce()){
    ta->badIfCond(myCond->line(), myCond->col());
    ta->nodeType(this, ErrorType::produce());
  }
  else{
    for(auto stmt : *myBodyTrue){
      stmt->typeAnalysis(ta);
    }
    for(auto stmt : *myBodyFalse){
      stmt->typeAnalysis(ta);
    }
  }
  ta->nodeType(this, BasicType::produce(VOID));
}
// literally identical to IfStmtNode
void WhileStmtNode::typeAnalysis(TypeAnalysis *ta){
  myCond->typeAnalysis(ta);
  if (ta->nodeType(myCond)->asFn() && (ta->nodeType(myCond)->asFn()->getReturnType() != BasicType::produce(BOOL))){
      // then its a function and we need to see if its return type is bool.
      ta->badIfCond(myCond->line(), myCond->col());
      ta->nodeType(this, ErrorType::produce());
      return;
  }
  else if (ta->nodeType(myCond) != BasicType::produce(BOOL) && ta->nodeType(myCond)!= ErrorType::produce()){
    ta->badIfCond(myCond->line(), myCond->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  else{
    for(auto stmt : *myBody){
      stmt->typeAnalysis(ta);
    }
    ta->nodeType(this, BasicType::produce(VOID));
    return;
  }
}

// ^
void RefNode::typeAnalysis(TypeAnalysis *ta){
  myID->typeAnalysis(ta);
  const DataType* this_ptr = ta->nodeType(myID); // returning the type of it's ID makes sense here. 
  if(this_ptr->asPtr()){
    const DataType * incremented_type = this_ptr->asPtr()->incLevel(); 
    ta->nodeType(this, incremented_type);
    return;
  }
  else{
    DataType* new_ptr = PtrType::produce(ta->nodeType(myID)->asBasic(), 1);
    ta->nodeType(this, new_ptr);
    return;
  }
}

void DerefNode::typeAnalysis(TypeAnalysis *ta){
  myID->typeAnalysis(ta);
  const DataType* this_ptr = ta->nodeType(myID); // some pointer
  if(this_ptr->asPtr()){
    const DataType * decremented_type = this_ptr->asPtr()->decLevel(); 
    ta->nodeType(this, decremented_type);
    return;
  } else if(this_ptr->asFn()) {
    ta->fnDeref(myID->line(), myID->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  else{
    /* To: Dr. Davidson
     * From: Clay & Evan
     * Hey, this particular error is not in the 
     * type_analysis.hpp. We made up our own error. 
     * Please grade accordingly :)
    */
    ta->baseDeref(myID->line(), myID->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
}

void IndexNode::typeAnalysis(TypeAnalysis *ta){
  myBase->typeAnalysis(ta);
  myOffset->typeAnalysis(ta);
  const DataType *deref_base = PtrType::derefType(ta->nodeType(myBase));

  if(!ta->nodeType(myOffset)->isInt()){
    ta->badIndex(myOffset->line(), myOffset->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  else if (!ta->nodeType(myBase)->isPtr()){
    ta->badPtrBase(myBase->line(), myBase->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  else{
    ta->nodeType(this, deref_base);
    return;
  }
}

void BinaryExpNode::typeAnalysis(TypeAnalysis *ta){
  // override in subclass
}

// Need to throw errors when:
// 1. Returning from a non-void function with a plain return statement
// 2. Returning a value from a void function
// 3. Returning a value of the wrong type from a non-void function
void ReturnStmtNode::typeAnalysis(TypeAnalysis *ta){
  const FnType * fn_type = ta->getCurrentFnType();
  // This error throws when you return nothing but the function isn't void.
  if(myExp == nullptr){
    if (fn_type->getReturnType() != BasicType::produce(VOID)){
      ta->badNoRet(this->line(), this->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
    return;
  } else {
    myExp->typeAnalysis(ta);
    // This error throws when you return something but the function is void.
    if(fn_type->getReturnType() == BasicType::produce(VOID)){
      ta->extraRetValue(this->myExp->line(), this->myExp->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
  }
  // - ta->nodeType(myExp) <-- VarType
  // - fn_type) <-- FnType
  if (ta->nodeType(myExp) != fn_type->getReturnType()){
    ta->badRetValue(this->myExp->line(), this->myExp->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  ta->nodeType(this, ta->nodeType(myExp)); // we good otherwise
}
void TypeNode::typeAnalysis(TypeAnalysis *ta){
  // just ignore this..
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis *ta){
  myLVal->typeAnalysis(ta);
  if(ta->nodeType(myLVal) != BasicType::produce(INT)){
    ta->badMathOpd(this->myLVal->line(), this->myLVal->col());
    return;
  }
  ta->nodeType(this, ta->nodeType(myLVal));
}

void PostDecStmtNode::typeAnalysis(TypeAnalysis *ta){
  myLVal->typeAnalysis(ta);
  if (ta->nodeType(myLVal) != BasicType::produce(INT)){
    ta->badMathOpd(this->myLVal->line(), this->myLVal->col());
    return;
  }
  ta->nodeType(this,ta->nodeType(myLVal));
}

void IntTypeNode::typeAnalysis(TypeAnalysis *ta){
  if(isPtr){
    ta->nodeType(this, PtrType::produce(BasicType::INT(), 1));
  } else {
    ta->nodeType(this, BasicType::INT());
  }
}

void BoolTypeNode::typeAnalysis(TypeAnalysis *ta){
  if(isPtr) {
    ta->nodeType(this, PtrType::produce(BasicType::BOOL(), 1));
  } else {
    ta->nodeType(this, (BasicType::BOOL()));
  }
}

void CharTypeNode::typeAnalysis(TypeAnalysis *ta){
  if(isPtr) {
    ta->nodeType(this, PtrType::produce(BasicType::CHAR(), 1));
  } else {
    ta->nodeType(this, (BasicType::CHAR()));
  }
}

void StrLitNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, PtrType::produce(BasicType::CHAR(), 1));
}

void IntLitNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, BasicType::produce(INT));
}

void CharLitNode::typeAnalysis(TypeAnalysis * ta){
  ta->nodeType(this, BasicType::produce(CHAR));
}

void NullPtrNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, BasicType::produce(VOID));
}

void TrueNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, BasicType::produce(BOOL));
}

void CallStmtNode::typeAnalysis(TypeAnalysis *ta)
{
  myCallExp->typeAnalysis(ta);
  ta->nodeType(this, ta->nodeType(myCallExp));
}

void CallExpNode::typeAnalysis(TypeAnalysis *ta)
{
  DataType *data_type = myID->getSymbol()->getDataType(); // FnType < DataType
  if(!data_type->asFn()){
    ta->badCallee(this->line(), this->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  const FnType* myFunction = data_type->asFn();
  // we need to compare types of myArgs and
  // myFunction->getFormalTypes(). They must all match,
  // else it is a badArgMatch.

  if(myFunction->getFormalTypes()->size() != myArgs->size())
  {
    ta->badArgCount(this->line(), this->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  }

  int idx = 0;
  for(auto formal_type : *(myFunction->getFormalTypes())){
    at(myArgs, idx)->typeAnalysis(ta); // call typeAnalysis for given argument.
    if (formal_type != ta->nodeType(at(myArgs, idx))){
      ta->badArgMatch(at(myArgs, idx)->line(), at(myArgs, idx)->col()); // types don't line up right.
      ta->nodeType(this, ErrorType::produce());
      return;
    }
    idx+=1;
  }

  // its type should = myID->getReturnType();
  ta->nodeType(this, myID->getSymbol()->getDataType()->asFn()->getReturnType());
}

void NegNode::typeAnalysis(TypeAnalysis *ta){
  // Make sure myExp is of Int type
  myExp->typeAnalysis(ta);
  if (ta->nodeType(myExp) != BasicType::produce(INT)){
    ta->badMathOpd(myExp->line(), myExp->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  } else {
    // Or return VOID, don't think it matters
    ta->nodeType(this, BasicType::produce(INT));
    return;
  }
}

void NotNode::typeAnalysis(TypeAnalysis *ta){
  myExp->typeAnalysis(ta);
  // Make sure myExp is of boolean type
  if(ta->nodeType(myExp) != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp->line(), myExp->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  } else {
    // do good things
    ta->nodeType(this, BasicType::produce(VOID));
    return;
  }
}

void PlusNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  }
  else { // meaning both types ARE INTs
    ta->nodeType(this, BasicType::produce(INT));
    return;
  }
}

void MinusNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  }  else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(INT));
    return;
  }
}

void TimesNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(INT));
    return;
  }
}

void DivideNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badMathOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(INT));
    return;
  }
}

void AndNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(BOOL)) || (rhs != BasicType::produce(BOOL))){
    if (lhs != BasicType::produce(BOOL)){
      ta->badLogicOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if(rhs != BasicType::produce(BOOL)){
      ta->badLogicOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void OrNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(BOOL)) || (rhs != BasicType::produce(BOOL))){
    if (lhs != BasicType::produce(BOOL)){
      ta->badLogicOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(BOOL)){
      ta->badLogicOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void EqualsNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if (lhs != rhs){
    ta->badEqOpr(this->line(), this->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void NotEqualsNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if (lhs != rhs){
    ta->badEqOpr(this->line(), this->col());
    ta->nodeType(this, ErrorType::produce());
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void LessNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void LessEqNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void GreaterNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
    // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}

void GreaterEqNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *lhs = ta->nodeType(myExp1);
  const DataType *rhs = ta->nodeType(myExp2);
  if ((lhs != BasicType::produce(INT)) || (rhs != BasicType::produce(INT))){
    if (lhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp1->line(), myExp1->col());
      ta->nodeType(this, ErrorType::produce());
    }
    if (rhs != BasicType::produce(INT)){
      ta->badRelOpd(myExp2->line(), myExp2->col());
      ta->nodeType(this, ErrorType::produce());
    }
    return;
  } else {
  // if you made it this far you passed.
    ta->nodeType(this, BasicType::produce(BOOL));
    return;
  }
}
}// end of namespace
