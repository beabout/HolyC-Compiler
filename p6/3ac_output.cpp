#include "ast.hpp"

// OpdWidth = { ADDR, QUADWORD, BYTE }
namespace holeyc{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	Procedure* new_proc = prog->makeProc(myID->getName());
  size_t param_idx = 0;
  for (auto formal : *myFormals){
    param_idx += 1;
    formal->to3AC(new_proc);
  	SymOpd *op = new_proc->getSymOpd(formal->ID()->getSymbol());
    GetArgQuad * formal_quad = new GetArgQuad(param_idx,op); // int indexIn, opd* opdIn
    new_proc->addQuad(formal_quad);
  }

  /* HERE: do getarg stuff for myFormals */
  for(auto statement : *myBody){
	  statement->to3AC(new_proc);
  }
}

void FnDeclNode::to3AC(Procedure * proc){
	//This never needs to be implemented,
	// the function only exists because of 
	// inheritance needs
}

void FormalDeclNode::to3AC(IRProgram * prog){
  SemSymbol *sym = ID()->getSymbol();
  if (sym == nullptr){
    throw new InternalError("null sym");
  }
  prog->gatherGlobal(sym);
}

void FormalDeclNode::to3AC(Procedure * proc){
  SemSymbol *sym = ID()->getSymbol();
  if (sym == nullptr){
    throw new InternalError("null sym");
  }
  proc->gatherFormal(sym);
}

Opd * IntLitNode::flatten(Procedure * proc){
	return new LitOpd(std::to_string(myNum), QUADWORD);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * CharLitNode::flatten(Procedure * proc){
	return new LitOpd(std::to_string(myVal), QUADWORD);
}

Opd * NullPtrNode::flatten(Procedure * proc){
	return new LitOpd("0", ADDR);
}

Opd * TrueNode::flatten(Procedure * proc){
	return new LitOpd("1", proc->getProg()->opWidth(this));
}

Opd * FalseNode::flatten(Procedure * proc){
	return new LitOpd("0", proc->getProg()->opWidth(this));
}

Opd * AssignExpNode::flatten(Procedure * proc){
  Opd * tmp = mySrc->flatten(proc);
  Opd * lhs = myDst->flatten(proc);
  if(mySrc->nodeKind() == "CallExp"){
    // then we need to do temp = getres
    GetRetQuad * ret = new GetRetQuad(tmp);    
    proc->addQuad(ret);
  }
  AssignQuad* quad = new AssignQuad(lhs, tmp);
  proc->addQuad(quad);
  return tmp;
}

Opd * LValNode::flatten(Procedure * proc){
	TODO(Implement me)
  // piazza says not to worry about this.
}

Opd * DerefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * RefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * CallExpNode::flatten(Procedure * proc){
  // c = foo(1); <-- WE NEED A tmp
  // foo(1); <-- WE DON'T NEED A tmp
  size_t arg_idx = 0;
  for(auto arg : *myArgs){
    arg_idx+=1;
    SetArgQuad * quad = new SetArgQuad(arg_idx, arg->flatten(proc));
    proc->addQuad(quad);
  }
  CallQuad* call = new CallQuad(myID->getSymbol());
  proc->addQuad(call);
  Opd * tmp = proc->makeTmp(proc->getProg()->opWidth(this));
  return tmp;
}

Opd * NegNode::flatten(Procedure * proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  UnaryOpQuad *quad = new UnaryOpQuad(dst, UnaryOp::NEG, myExp->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd * NotNode::flatten(Procedure * proc){
	Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  UnaryOpQuad *quad = new UnaryOpQuad(dst, UnaryOp::NOT, myExp->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd * PlusNode::flatten(Procedure * proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::ADD, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd * MinusNode::flatten(Procedure * proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::SUB, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd * TimesNode::flatten(Procedure * proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::MULT, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd * DivideNode::flatten(Procedure * proc){
  Opd * dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad * quad = new BinOpQuad(dst, BinOp::DIV, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd * AndNode::flatten(Procedure * proc){
  Opd * dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad * quad = new BinOpQuad(dst, BinOp::AND, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd *OrNode::flatten(Procedure *proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::OR, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd *EqualsNode::flatten(Procedure *proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::EQ, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd *NotEqualsNode::flatten(Procedure *proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::NEQ, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd *LessNode::flatten(Procedure *proc){
  Opd *dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(dst, BinOp::LT, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

Opd *GreaterNode::flatten(Procedure *proc){
  Opd * dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad * quad = new BinOpQuad(dst, BinOp::GT, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;}

Opd *LessEqNode::flatten(Procedure *proc){
  Opd * dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad * quad = new BinOpQuad(dst, BinOp::LTE, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;}

Opd *GreaterEqNode::flatten(Procedure *proc){
  Opd * dst = proc->makeTmp(proc->getProg()->opWidth(this));
  BinOpQuad * quad = new BinOpQuad(dst, BinOp::GTE, myExp1->flatten(proc), myExp2->flatten(proc));
  proc->addQuad(quad);
  return dst;
}

void AssignStmtNode::to3AC(Procedure *proc){
  myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure *proc){
  LitOpd* one = new LitOpd("1", proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(myLVal->flatten(proc), BinOp::ADD, myLVal->flatten(proc),one);
  proc->addQuad(quad);
}

void PostDecStmtNode::to3AC(Procedure *proc){
  LitOpd * one = new LitOpd("1", proc->getProg()->opWidth(this));
  BinOpQuad *quad = new BinOpQuad(myLVal->flatten(proc), BinOp::SUB, myLVal->flatten(proc), one);
  proc->addQuad(quad);
}

void FromConsoleStmtNode::to3AC(Procedure *proc){
  IntrinsicQuad *input_quad = new IntrinsicQuad(Intrinsic::INPUT, myDst->flatten(proc));
  proc->addQuad(input_quad);
}

void ToConsoleStmtNode::to3AC(Procedure *proc){
  IntrinsicQuad * output_quad = new IntrinsicQuad(Intrinsic::OUTPUT,mySrc->flatten(proc));
  proc->addQuad(output_quad);
}

void IfStmtNode::to3AC(Procedure *proc){
  Opd* my_opd = myCond->flatten(proc);
  // now ifz quad
  Label * my_lbl = proc->makeLabel();
  JmpIfQuad* j = new JmpIfQuad(my_opd , my_lbl); 
  proc->addQuad(j);
  for(auto stmt : *myBody){
    stmt->to3AC(proc);
  }
  NopQuad* nq = new NopQuad();
  nq->addLabel(my_lbl);
  proc->addQuad(nq);
  // now leave label
  // this should print -
  // lbl thing: nop
}

void IfElseStmtNode::to3AC(Procedure *proc){
  Opd *my_opd = myCond->flatten(proc); 
  Label * my_lbl = proc->makeLabel();
  JmpIfQuad* j = new JmpIfQuad(my_opd, my_lbl); 
  proc->addQuad(j);
  for(auto stmt : *myBodyTrue){
    stmt->to3AC(proc);
  }
  Label *my_lbl_2 = proc->makeLabel();
  JmpQuad *j2 = new JmpQuad(my_lbl_2);
  proc->addQuad(j2);

  // beginning of else
  NopQuad* nq = new NopQuad();
  nq->addLabel(my_lbl);
  proc->addQuad(nq);

  for(auto stmt : *myBodyFalse){
    stmt->to3AC(proc);
  }
  
  // end of if
  NopQuad* nq2 = new NopQuad();
  nq2->addLabel(my_lbl_2);
  proc->addQuad(nq2);
}

void WhileStmtNode::to3AC(Procedure * proc){
  NopQuad *nq = new NopQuad();
  Label *start = proc->makeLabel(); // EG: changed from getLeaveLabel
  Label *end = proc->makeLabel(); // EG: changed from getLeaveLabel
  nq->addLabel(start);
  proc->addQuad(nq);
  Opd * my_opd = myCond->flatten(proc);

  // jump if to skip to end
  JmpIfQuad* j = new JmpIfQuad(my_opd, end); 
  proc->addQuad(j);


  for(auto stmt : *myBody){
    stmt->to3AC(proc);
  }

  // jumpifquad back to first label
  JmpQuad* j2 = new JmpQuad(start); 
  proc->addQuad(j2);

  // end of while
  NopQuad *nq2 = new NopQuad();
  nq2->addLabel(end);
  proc->addQuad(nq2);

}

void CallStmtNode::to3AC(Procedure *proc){
  myCallExp->flatten(proc);
}

void ReturnStmtNode::to3AC(Procedure *proc){
  SetRetQuad * quad = nullptr;
  if(myExp == nullptr){
    quad = new SetRetQuad(new AuxOpd("", proc->getProg()->opWidth(this)));
  }else{
    quad = new SetRetQuad(myExp->flatten(proc));
  }
  proc->addQuad(quad);
  // add jump quad with proper label.
  JmpQuad* j = new JmpQuad(proc->getLeaveLabel());
  proc->addQuad(j);
}

void VarDeclNode::to3AC(Procedure *proc)
{
  SemSymbol *sym = ID()->getSymbol();
  if (sym == nullptr)
  {
    throw new InternalError("null sym");
  }
  proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram *prog){
  SemSymbol *sym = ID()->getSymbol();
  if (sym == nullptr){
    throw new InternalError("null sym");
  }

  prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd *IDNode::flatten(Procedure *proc){
  // for now do nothing?
  SemSymbol * sym = getSymbol();
  SymOpd * id_sym = new SymOpd(sym, proc->getProg()->opWidth(this));
  return id_sym;
}

}// end of file
