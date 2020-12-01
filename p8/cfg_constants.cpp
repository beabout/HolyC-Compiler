#include "cfg_constants.hpp"

using namespace holeyc;

bool ConstantsAnalysis::runGraph(ControlFlowGraph * cfg){
	//This function should iterate over each block, much like the
	// dead code elimination pass. However, constant propagation
	// is a FORWARD analysis, so the inFacts will be made up
	// of the union of PREDECESSORS, rather than the union of SUCCESSORS
	//TODO(implement me!)

	// cfg has:
	//std::set<BasicBlock *> blockSuccessors(BasicBlock * block);
	//std::set<BasicBlock *> blockPredecessors(BasicBlock * block);

  Procedure * proc = cfg->getProc();
  IRProgram * prog = proc->getProg();
  std::set<Opd *> globalSyms = prog->globalSyms();
  
  // initialize ConstantsFacts for every block
  for (BasicBlock *block : *cfg->getBlocks()){
    ConstantsFacts empty;
    inFacts[block] = empty;
    outFacts[block] = empty;
  }

  bool changed = true;
  int count = 0; 
  while(changed) {
    changed = false;
    cout << "\ntime running: " << ++count << endl;
    for(auto block : *cfg->getBlocks()) {
      ConstantsFacts in = inFacts[block];

      // Block's ConstantFacts: add to blockSuccessors ConstantsFacts
      for (BasicBlock *block : cfg->blockPredecessors(block)) {
        in.addFacts(outFacts[block]);
      }
      // Block's ConstantFacts: add global variables constants with TOPVAL
      for(auto sym : globalSyms){ 
        ConstantVal cVal; 
        cVal.setTop();
        in.gen(sym, cVal);
      }
      // Block's ConstantFacts: add formal constants with TOPVAL
      for (auto formal : proc->getFormals()){
        ConstantVal cVal;
        cVal.setTop();
        in.gen(formal, cVal);
      }
      // Block's ConstantFacts: add local variables constants with TOPVAL
      for (auto local : proc->getLocals()){
        ConstantVal cVal;
        cVal.setTop();
        in.gen(local.second, cVal);
      }
      // Block's ConstantFacts: add temps? variables constants with TOPVAL
      for (auto temp : proc->getTemps()){
        ConstantVal cVal;
        cVal.setTop();
        in.gen(temp, cVal);
      }

      inFacts[block] = in;
      bool blockChange = runBlock(cfg, block);
      if(blockChange){
        changed = true;
      }
    }
  }
  return changed;
}

bool ConstantsAnalysis::runBlock(ControlFlowGraph * cfg, BasicBlock * block){
	// This function should iterate over each quad in the block, 
	// much like the dead code elimination pass, except that this
	// is a forward analysis, so you should iterate over the block
	// from the first quad to the last, collecting the known constants

  // There are two things you should do for each quad:
  // 1) Constant Folding
  // if all source operands are constants (i.e. can cast to LitOpd),
  // evaluate the result and replace the quad with a simple assignment
  // 2) Constant Propagation
  // if the source operand of an assignment is a constant, record
  // that fact and keep rolling
  // if the source operand is a variable (SymOpd or AuxOpd), but that
  // variable is known to be constant, replace the

  Procedure * proc = cfg->getProc();
  IRProgram * ir = proc->getProg();

  // obtain ConstantFacts of the incoming ConstantFacts and set outFacts at end of function.
  ConstantsFacts in = inFacts[block];
  ConstantsFacts myOutFacts;
  
  bool changed = false;
  for(Quad* quad : *(block->getQuads())){
    // 1) Constant Folding
    if (auto q = dynamic_cast<BinOpQuad *>(quad)){
      // then we know it has source operands.
      // if both src1 and src2 are constants, we can fold.
      Opd* src1 = q->getSrc1();
      Opd* src2 = q->getSrc2();
      Opd *dest = q->getDst();
      if(auto op1 = dynamic_cast<LitOpd *>(src1)){
        if(auto op2 = dynamic_cast<LitOpd*>(src2)){
          changed = true;
          std::string value_1 = op1->valString();
          std::string value_2 = op2->valString();
          int ival; 
          bool bval1; 
          bool bval2;
          bool bval;
          LitOpd* value_to_assign = nullptr;
          AssignQuad* assign_quad = nullptr;
          switch (q->getOp()){
            case BinOp::ADD:
              ival = std::stoi(value_1) + std::stoi(value_2);
              value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::SUB:
              ival = std::stoi(value_1) - std::stoi(value_2);
              value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::DIV:
              // we don't wanna divide by 0
              if(!(std::stoi(value_2) == 0)){
                ival = std::stoi(value_1) / std::stoi(value_2);
                value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
                assign_quad = new AssignQuad(dest, value_to_assign);
                proc->replaceQuad(quad, assign_quad);
                block->replaceQuad(quad, assign_quad);
              }
              break;
            case BinOp::MULT:
              ival = std::stoi(value_1) * std::stoi(value_2); 
              value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::OR:
              bval1 = (value_1 != "0");
              bval2 = (value_2 != "0");
              bval = bval1 || bval2;
              value_to_assign = new LitOpd(std::to_string(bval), dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              std::cout << "Replaced an OR quad with a LitOpd.\n";
              break;
            case BinOp::AND:
              bval1 = (value_1 != "0");
              bval2 = (value_2 != "0");
              bval = bval1 && bval2;
              value_to_assign = new LitOpd(std::to_string(bval), dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              std::cout << "Replaced an AND quad with a LitOpd.\n";
              break;
            case BinOp::EQ:
              if(value_1 == value_2){
                // result is true.
                // replace this quad with "assign true" quad.
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                // result is false.
                // replace this quad with "assign false" quad.
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::NEQ:
              if(value_1 != value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::LT:
              if(value_1 < value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::GT:
              if(value_1 > value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::LTE:
              if(value_1 <= value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
              break;
            case BinOp::GTE:
              if(value_1 >= value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              proc->replaceQuad(quad, assign_quad);
              block->replaceQuad(quad, assign_quad);
            default:
              break;
          }
        }
      }
    }
    
    // 2) Constant Propogation
    if (auto q = dynamic_cast<AssignQuad *>(quad)) {
      int t = 1;
      // update outFacts with value
      cout << "AssignQuad\n";
      if(LitOpd * lit_opd = dynamic_cast<LitOpd *>(q->getSrc())){
        cout << "src is literal\n";
        // [tmp0] = 1
        ConstantVal new_value = ConstantVal(lit_opd);
        inFacts[block].updateVal(q->getDst(), new_value);
      } else {
        cout << "src is ID\n";
        // [a] = [tmp0]
        // [tmp1] := 0 AND8 [tmp0]
        ConstantVal new_value = inFacts[block].getVal(q->getSrc());
        inFacts[block].updateVal(q->getDst(), new_value);
        LitOpd *value_to_assign = nullptr; 
        switch(new_value.getType()){
          case INTVAL:
            value_to_assign = new LitOpd(std::to_string(new_value.intVal), q->getDst()->getWidth());
            break; 
          case CHARVAL: 
            value_to_assign = new LitOpd(std::to_string(new_value.charVal), q->getDst()->getWidth());
            break;
          case BOOLVAL:
            value_to_assign = new LitOpd(std::to_string(new_value.boolVal), q->getDst()->getWidth());
            break; 
          case TOPVAL: 
            cout << "Src Val [TOPVAL] This value was never updated.\n";
            break; 

        }
        if(value_to_assign != nullptr){
          changed = true;
          AssignQuad* assign_quad = new AssignQuad(q->getDst(), value_to_assign);
          proc->replaceQuad(quad, assign_quad);
          block->replaceQuad(quad, assign_quad);
        }
      }

      inFacts[block].printFacts();

    } else if (auto q = dynamic_cast<BinOpQuad *>(quad)) {
      cout << "BinOpQuad\n";
      // [a] = [b] AND8 [tmp0]
      LitOpd* src1_lit = dynamic_cast<LitOpd *>(q->getSrc1());
      LitOpd* src2_lit = dynamic_cast<LitOpd *>(q->getSrc2());
      LitOpd *value_to_assign_1 = nullptr;
      LitOpd *value_to_assign_2 = nullptr; 
      if(src1_lit == nullptr){
        ConstantVal new_value = inFacts[block].getVal(q->getSrc1()); // expect value to be in inFacts
        switch(new_value.getType()){
          case INTVAL:
            changed = true;
            value_to_assign_1 = new LitOpd(std::to_string(new_value.intVal), q->getDst()->getWidth());
            q->setSrc1(value_to_assign_1);
            break; 
          case CHARVAL:
            changed = true;
            value_to_assign_1 = new LitOpd(std::to_string(new_value.charVal), q->getDst()->getWidth());
            q->setSrc1(value_to_assign_1);
            break; 
          case BOOLVAL:
            changed = true;
            value_to_assign_1 = new LitOpd(std::to_string(new_value.boolVal), q->getDst()->getWidth());
            q->setSrc1(value_to_assign_1);
            break; 
          case TOPVAL: 
            cout << "Src1 is not LitOpd [TOPVAL] This value was never updated.\n";
            break;
        }
      }
      if(src2_lit == nullptr) {
        ConstantVal new_value = inFacts[block].getVal(q->getSrc2());
        switch(new_value.getType()){
          case INTVAL:
            changed = true;
            value_to_assign_2 = new LitOpd(std::to_string(new_value.intVal), q->getDst()->getWidth());
            q->setSrc2(value_to_assign_2);
            break; 
          case CHARVAL: 
            changed = true;
            value_to_assign_2 = new LitOpd(std::to_string(new_value.charVal), q->getDst()->getWidth());
            q->setSrc2(value_to_assign_2);
            break; 
          case BOOLVAL:
            changed = true;
            value_to_assign_2 = new LitOpd(std::to_string(new_value.boolVal), q->getDst()->getWidth());
            q->setSrc2(value_to_assign_2);
            break; 
          case TOPVAL:
            cout << "Src2 is not LitOpd [TOPVAL] This value was never updated.\n";
            break; 
        }
      }
      // Not sure if this works, but hope this will actually replace quad. - Evan
      if(value_to_assign_1 != nullptr || value_to_assign_2 != nullptr){
        BinOpQuad *new_bin_quad = q;
        proc->replaceQuad(quad, new_bin_quad);
        block->replaceQuad(quad, new_bin_quad);
      }
      
    } else if (auto q = dynamic_cast<UnaryOpQuad *>(quad)) {
      cout << "UnaryOpQuad\n";
      LitOpd* src_lit = dynamic_cast<LitOpd *>(q->getSrc());
      if(src_lit == nullptr){
        LitOpd *value_to_assign = nullptr;
        bool inverse_bval;
        ConstantVal new_value = inFacts[block].getVal(q->getSrc());
        switch (new_value.getType()){
        case INTVAL:
          // it is a NEG64
          changed = true;
          value_to_assign = new LitOpd(std::to_string(new_value.intVal), q->getDst()->getWidth());
          q->setSrc(value_to_assign);
          break;
        case BOOLVAL:
          // it is a NOT8
          changed = true;
          // inverse_bval = !new_value.boolVal;
          // value_to_assign = new LitOpd(std::to_string(inverse_bval), q->getDst()->getWidth());
          value_to_assign = new LitOpd(std::to_string(new_value.boolVal), q->getDst()->getWidth());
          q->setSrc(value_to_assign);
          break;
        default:
          cout << "Src is not LitOpd [TOPVAL, CHARVAL] This value was never updated.\n";
          break;
        }
        
        UnaryOpQuad * un_op_quad = q;
        proc->replaceQuad(quad, un_op_quad);
        block->replaceQuad(quad, un_op_quad);
      }
    } else if (auto q = dynamic_cast<SetArgQuad *>(quad)) {

    }
  }
  outFacts[block].addFacts(inFacts[block]); // update my outfacts.
  return changed;
}
