#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

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
	//TODO: Note that this function is incomplete. 
	// and needs additional code

	//Do typeAnalysis on the subexpressions
	myDst->typeAnalysis(ta);
	mySrc->typeAnalysis(ta);

	const DataType * tgtType = ta->nodeType(myDst);
	const DataType * srcType = ta->nodeType(mySrc);

	//While incomplete, this gives you one case for 
	// assignment: if the types are exactly the same
	// it is usually ok to do the assignment. One
	// exception is that if both types are function
	// names, it should fail type analysis
	if (tgtType == srcType){
    // if (myDst->isFn()){
    // } else {  
    //   ta->nodeType(this, tgtType);
    //   return;
    // }
	}

	
	//Some functions are already defined for you to
	// report type errors. Note that these functions
	// also tell the typeAnalysis object that the
	// analysis has failed, meaning that main.cpp
	// will print "Type check failed" at the end
	ta->badAssignOpr(this->line(), this->col());


	//Note that reporting an error does not set the
	// type of the current node, so setting the node
	// type must be done
	ta->nodeType(this, ErrorType::produce());
}

void DeclNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis * ta){
	// VarDecls always pass type analysis, since they 
	// are never used in an expression. You may choose
	// to type them void (like this), as discussed in class
  myID->typeAnalysis(ta);
	ta->nodeType(this, BasicType::produce(VOID));
  
}

void IDNode::typeAnalysis(TypeAnalysis * ta){
	// IDs never fail type analysis and always
	// yield the type of their symbol (which
	// depends on their definition)
	ta->nodeType(this, this->getSymbol()->getDataType());
}

void LValNode::typeAnalysis(TypeAnalysis * ta){
  // override in subclass
}

void FromConsoleStmtNode::typeAnalysis(TypeAnalysis *ta){
  myDst->typeAnalysis(ta);
  ta->nodeType(this, BasicType::produce(VOID)); // <- I suppose this is void..
}

void ToConsoleStmtNode::typeAnalysis(TypeAnalysis *ta){
  mySrc->typeAnalysis(ta);
  if (ta->nodeType(mySrc) != PtrType::produce(BasicType::CHAR(), 0) || ta->nodeType(mySrc) != BasicType::produce(CHAR)){
    // error
    // TODO: what error to report??
    ta->nodeType(this, ErrorType::produce());
    return;
  }
  ta->nodeType(this, BasicType::produce(VOID)); // <- I suppose this is void..
}

void IfStmtNode::typeAnalysis(TypeAnalysis *ta){
  myCond->typeAnalysis(ta);
  if(ta->nodeType(myCond) != BasicType::produce(BOOL)){
    ta->badIfCond(myCond->line(), myCond->col());
    ta->nodeType(this, ErrorType::produce());
  }
  else
  {
    	for(auto stmt : *myBody){
        stmt->typeAnalysis(ta);
	    }
  }
  ta->nodeType(this, BasicType::produce(VOID));
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis *ta){

}

void WhileStmtNode::typeAnalysis(TypeAnalysis *ta){

}

void RefNode::typeAnalysis(TypeAnalysis *ta){

}

void DerefNode::typeAnalysis(TypeAnalysis *ta){

}

void IndexNode::typeAnalysis(TypeAnalysis *ta){

}

void BinaryExpNode::typeAnalysis(TypeAnalysis *ta){
  // override in subclass
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis *ta){
	// This error throws when you return nothing but the function isn't void.
  if(myExp == nullptr){
    if (!(ta->getCurrentFnType()->isVoid())){
      ta->badNoRet(this->myExp->line(), this->myExp->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
  } else {
    myExp->typeAnalysis(ta);
    // This error throws when you return something but the function is void.
    if(ta->getCurrentFnType()->isVoid()){
      ta->extraRetValue(this->myExp->line(), this->myExp->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
    // - ta->nodeType(myExp) <-- VarType
    // - ta->getCurrentFnType()) <-- FnType
    if (ta->nodeType(myExp) != ta->getCurrentFnType()->getReturnType()){
      ta->badRetValue(this->myExp->line(), this->myExp->col());
      ta->nodeType(this, ErrorType::produce());
      return;
    }
      // else success
      ta->nodeType(this, ta->nodeType(myExp));
    }

  // This error throws when you return something with a type that doesn't match the expected function return type.
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

void IntLitNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, BasicType::produce(INT));
}

void CharLitNode::typeAnalysis(TypeAnalysis * ta){
  ta->nodeType(this, BasicType::produce(CHAR));
}

void StrLitNode::typeAnalysis(TypeAnalysis * ta){
  ta->nodeType(this, PtrType::produce(BasicType::CHAR(),0));
}

void NullPtrNode::typeAnalysis(TypeAnalysis * ta){
  ta->nodeType(this, BasicType::produce(VOID));
}

void TrueNode::typeAnalysis(TypeAnalysis * ta){
  ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis *ta){
  ta->nodeType(this, BasicType::produce(BOOL));
}

void CallStmtNode::typeAnalysis(TypeAnalysis *ta){

}

void CharTypeNode::typeAnalysis(TypeAnalysis *ta){

}

void FormalDeclNode::typeAnalysis(TypeAnalysis *ta){
  this->ID()->typeAnalysis(ta);
  ta->nodeType(this, BasicType::produce(VOID));
}

void NegNode::typeAnalysis(TypeAnalysis *ta){
  // Make sure myExp is of Int type
  myExp->typeAnalysis(ta);
  if (ta->nodeType(myExp) != BasicType::produce(INT)){
    ta->badMathOpd(this->myExp->line(), this->myExp->col());
    return;
  }
  ta->nodeType(this, ta->nodeType(myExp)); // Or return VOID, don't think it matters
}

void NotNode::typeAnalysis(TypeAnalysis *ta){
  myExp->typeAnalysis(ta);
  // Make sure myExp is of boolean type
  if(ta->nodeType(myExp) != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp->line(), myExp->col());
    return; 
  } else {
    // do good things
  }
}

void PlusNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);

  if (tgtType != BasicType::produce(INT)){
    ta->badMathOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(INT)){
    ta->badMathOpd(myExp2->line(), myExp2->col());
  }
  if (tgtType == srcType){
  } else {
  }
}

void MinusNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(INT)){
    ta->badMathOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(INT)){
    ta->badMathOpd(myExp2->line(), myExp2->col());
  }
  if (tgtType == srcType){
  } else {
  }
}

void TimesNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(INT)){
    ta->badMathOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(INT)){
    ta->badMathOpd(myExp2->line(), myExp2->col());
  }
  if (tgtType == srcType){
  } else {
  }
}

void DivideNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(INT)){
    ta->badMathOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(INT)){
    ta->badMathOpd(myExp2->line(), myExp2->col());
  }
  if (tgtType == srcType){
  } else {
  }
}

void AndNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if(tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if(srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void OrNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void EqualsNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void NotEqualsNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void LessNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void LessEqNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void GreaterNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

void GreaterEqNode::typeAnalysis(TypeAnalysis * ta){
  myExp1->typeAnalysis(ta);
  myExp2->typeAnalysis(ta);

  const DataType *tgtType = ta->nodeType(myExp1);
  const DataType *srcType = ta->nodeType(myExp2);
  if (tgtType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp1->line(), myExp1->col());
  }
  if (srcType != BasicType::produce(BOOL)){
    ta->badLogicOpd(myExp2->line(), myExp2->col());
  }

  if (tgtType == srcType){
  } else {
  }
}

}// end of namespace
