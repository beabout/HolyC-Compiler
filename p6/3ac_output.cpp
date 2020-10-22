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
    // SymOpd *op = new SymOpd(formal->ID()->getSymbol(), prog->opWidth(this));
    // SymOpd *op = new SymOpd(OpdWidth::QUADWORD);
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
	TODO(Implement me)
}

Opd * NullPtrNode::flatten(Procedure * proc){
	return new LitOpd("0", ADDR);
	TODO(Implement me)
}

Opd * TrueNode::flatten(Procedure * proc){
	//TODO(Implement me)
	return new LitOpd("1", proc->getProg()->opWidth(this));
}

Opd * FalseNode::flatten(Procedure * proc){
	//TODO(Implement me)
	return new LitOpd("0", proc->getProg()->opWidth(this));
}

Opd * AssignExpNode::flatten(Procedure * proc){
  // Still need to check if RHS is function. If so, need to: 
  //  - add setres
  //  - add a tmp
	Opd* rhs = mySrc->flatten(proc);
  Opd *dst = myDst->flatten(proc);
	AssignQuad* quad = new AssignQuad(dst, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * LValNode::flatten(Procedure * proc){
	TODO(Implement me)
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
  CallQuad* call = new CallQuad(  myID->getSymbol() );
  proc->addQuad(call);
  return proc->getSymOpd(myID->getSymbol());
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

void AssignStmtNode::to3AC(Procedure *proc)
{
  // TODO(Implement me)
  // flatten myExp
  // then do stuff
  // Opd* lhs = something
	// Opd* rhs = myExp->flatten(proc);
  // Opd *dst = new AuxOpd("todo",OpdWidth::QUADWORD);
	// AssignQuad* quad = new AssignQuad(dst, rhs);
  myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure *proc){
  // TODO(Implement me)
  myLVal->flatten(proc);
}

void PostDecStmtNode::to3AC(Procedure *proc)
{
  // TODO(Implement me)
  myLVal->flatten(proc);
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
  // [tmp2] : = [f] AND8 1 
  // IFZ[tmp2] GOTO lbl_2
  // setret 0 goto lbl_1
  // lbl_2 : nop
  // lbl_3 : nop
  NopQuad* nq = new NopQuad();
  nq->addLabel(my_lbl);
  proc->addQuad(nq);

  // now leave label
  // this should print -
  // lbl thing: nop
}

void IfElseStmtNode::to3AC(Procedure *proc){
  Opd* my_opd = myCond->flatten(proc); // [f] AND8 1
  Label * my_lbl = proc->makeLabel(); // lbl_2
  JmpIfQuad* j = new JmpIfQuad(my_opd, my_lbl); 
  proc->addQuad(j);
  for(auto stmt : *myBodyTrue){
    stmt->to3AC(proc);
  }
  
  // jump here also
  Label * my_lbl_2 = proc->makeLabel();
  JmpQuad* j2 = new JmpQuad(my_lbl_2);
  proc->addQuad(j2);

  NopQuad* nq = new NopQuad();
  nq->addLabel(my_lbl);
  proc->addQuad(nq);
  

  for(auto stmt : *myBodyFalse){
    stmt->to3AC(proc);
  }

  // will jump to here 
  NopQuad* nq2 = new NopQuad();
  nq->addLabel(my_lbl_2);
  proc->addQuad(nq2);
}

void WhileStmtNode::to3AC(Procedure *proc){
  NopQuad *nq = new NopQuad();
  Label *leave = proc->makeLabel(); // EG: changed from getLeaveLabel
  nq->addLabel(leave);
  proc->addQuad(nq);
  myCond->flatten(proc);
  for(auto stmt : *myBody){
    stmt->to3AC(proc);
  }
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

void VarDeclNode::to3AC(IRProgram *prog)
{
  SemSymbol *sym = ID()->getSymbol();
  if (sym == nullptr)
  {
    throw new InternalError("null sym");
  }

  prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd *IDNode::flatten(Procedure *proc){
  // for now do nothing?
  	return new LitOpd("[" + getName() + "]", proc->getProg()->opWidth(this));
}

}// end of file
