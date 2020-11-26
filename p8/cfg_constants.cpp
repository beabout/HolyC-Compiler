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
  bool still_changing = false; 
  for(auto block : *(cfg->getBlocks())){
    still_changing = runBlock(cfg, block) && still_changing;
  }
  return still_changing;
}

bool ConstantsAnalysis::runBlock(ControlFlowGraph * cfg, BasicBlock * block){
	//This function should iterate over each quad in the block, 
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
  bool still_changing = false;
  for(auto quad : *(block->getQuads())){
    // do shtuff

    // 1) Constant Folding
    if (auto q = dynamic_cast<BinOpQuad *>(quad)){
      // then we know it has source operands.
      // if both src1 and src2 are constants, we can fold.
      Opd* src1 = q->getSrc1();
      Opd* src2 = q->getSrc2();
      Opd *dest = q->getDst();
      if(auto op1 = dynamic_cast<LitOpd *>(src1)){
        if(auto op2 = dynamic_cast<LitOpd*>(src2)){
          still_changing = true;
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
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::SUB:
              ival = std::stoi(value_1) - std::stoi(value_2);
              value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::DIV:
              // we don't wanna divide by 0
              if(!(std::stoi(value_2) == 0)){
                ival = std::stoi(value_1) / std::stoi(value_2);
                value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
                assign_quad = new AssignQuad(dest, value_to_assign);
                cfg->getProc()->replaceQuad(quad, assign_quad);
              }
              break;
            case BinOp::MULT:
                ival = std::stoi(value_1) * std::stoi(value_2); 
                value_to_assign = new LitOpd(std::to_string(ival),dest->getWidth());
                assign_quad = new AssignQuad(dest, value_to_assign);
                cfg->getProc()->replaceQuad(quad, assign_quad);             
              break;
            case BinOp::OR:
              std::cout << "Value is: " << value_1 << std::endl; // test
              bval1 = (value_1 != "0");
              bval2 = (value_2 != "0");
              bval = bval1 || bval2;
              value_to_assign = new LitOpd(std::to_string(bval), dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::AND:
              bval1 = (value_1 != "0");
              bval2 = (value_2 != "0");
              bval = bval1 && bval2;
              value_to_assign = new LitOpd(std::to_string(bval), dest->getWidth());
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
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
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::NEQ:
              if(value_1 != value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::LT:
              if(value_1 < value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::GT:
              if(value_1 > value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::LTE:
              if(value_1 <= value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
              break;
            case BinOp::GTE:
              if(value_1 >= value_2){
                value_to_assign = new LitOpd("1", dest->getWidth());
              }else{
                value_to_assign = new LitOpd("0", dest->getWidth());
              }
              assign_quad = new AssignQuad(dest, value_to_assign);
              cfg->getProc()->replaceQuad(quad, assign_quad);
            default:
              break;
          }
        }
      }
    }
    
    // 2) Constant Propogation
    if (auto q = dynamic_cast<BinOpQuad *>(quad)){
      std::cout << "Binary op quad detected at propogation phase.\n";
      Opd* src1 = q->getSrc1();
      Opd* src2 = q->getSrc2();
      if (auto op1_sym = dynamic_cast<SymOpd *>(src1)) {
        // attempt to propogate op1.
        still_changing = true;
        cout << op1_sym->getSym()->getName() << endl;
      }
      else if (auto op1_aux = dynamic_cast<AuxOpd *>(src1)) {
        still_changing = true;
        cout << op1_aux->getName() << endl;
      }
      
      if(auto op2_sym = dynamic_cast<SymOpd*>(src2)) {
        // attempt to propogate op2.
        still_changing = true;
        cout << op2_sym->getSym()->getName() << endl;
      }
      else if (auto op2_aux = dynamic_cast<AuxOpd *>(src1)) {
        still_changing = true;
        cout << op2_sym->getSym()->getName() << endl;
      }
    } else if (auto q = dynamic_cast<UnaryOpQuad *>(quad)){


    } else if (auto q = dynamic_cast<AssignQuad *>(quad)) {


    } else if (auto q = dynamic_cast<SetArgQuad *>(quad)){


    }

    //if([tmp]) ?

    // 2) Constant Propagation
    // here i think - evan
    // where can temps occur?
    // - BinOpQuad
    // - UnarOpQuad
    // - AssignQuad
    // - CallQuad?

    // a  = [tmp]
    // a = 1 + [tmp]
    // a = ![tmp]
    // setarg 1 [tmp]
  }

    return still_changing;
  }
