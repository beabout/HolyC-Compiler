#ifndef HOLEYC_CFG_CONSTANTS
#define HOLEYC_CFG_CONSTANTS

#include <map>
#include "cfg.hpp"
#include "3ac.hpp"

namespace holeyc{

enum ConstantValType {INTVAL, CHARVAL, BOOLVAL, TOPVAL};

/**
* This class represents the possible value that an Opd might take on at
* at some program point. Note that if the Opd might take on more than 
* one possible value at a program point, it is simply represented as
* TOPVAL (regardless of what the values are). This use of TOPVAL causes the 
* algorithm to reach saturation much faster. 
**/
class ConstantVal{
public:
  	ConstantVal(){};
    ConstantVal(LitOpd * lit){
      std::string value = lit->valString();
      if(value.length() != 1){
        intVal = std::stoi(value);
        type = INTVAL;
      } else {
        // bool or char
        if(value == "0") {
          boolVal = false;
          type = BOOLVAL;
        } else if (value == "1") {
          boolVal = true;
          type = BOOLVAL;
        } else {
          charVal = value[0];
          type = CHARVAL;
        }
      }
    }

	ConstantValType type;

	int intVal;
	char charVal;
	bool boolVal;
	void setInt(int val){ intVal = val; type = INTVAL; } 
	void setBool(bool val){ boolVal = val; type = BOOLVAL; } 
	void setChar(char val){ charVal = val; type = CHARVAL; } 
	void setTop(){ type = TOPVAL; } // we don't know it's value yet. 
  ConstantValType getType() { return type; }


	void merge(ConstantVal other){
		if (other.type == TOPVAL) {
			setTop(); 
		} else if (other.type == INTVAL) { 
			if (intVal != other.intVal){ setTop(); }
		} else if (other.type == CHARVAL) { 
			if (charVal != other.charVal){ setTop(); }
		} else if (other.type == BOOLVAL) { 
			if (intVal != other.intVal){ setTop(); }
		}
	}

  std::string valToString(){
    std::string s = "";
    switch(type){
      case INTVAL:
        s += std::to_string(intVal);
        break;
      case CHARVAL: 
        s += std::to_string(charVal);
        break;
      case BOOLVAL:
        s += std::to_string(boolVal);
        break;
      case TOPVAL:
        s += "TOPVAL";
        break;
    }
    return s;
  }
};

class ConstantsFacts{
public:
	ConstantsFacts(){}
	ConstantsFacts clone(){
		ConstantsFacts facts;
		return facts;
	}

  std::map<Opd *, ConstantVal> Vals(){return vals;};

  void addFacts(ConstantsFacts &other) { 
    for(auto pair : other.vals){
      gen(pair.first, pair.second);
    }
  }

  void merge(ConstantsFacts &other) {
    for(auto pair : other.vals){
      // if find pair in this->vals, set to top.
      // otherwise, just accept it.
      std::map<Opd *, ConstantVal>::iterator it = this->vals.find(pair.first);
      if(it != vals.end()) { 
        // opd to add already exists in Facts
        if(it->second.getType() != TOPVAL && pair.second.getType() != TOPVAL){
          ConstantVal t; 
          t.setTop();
          it->second = t;
        } else {
          it->second = pair.second;
        }
      } else {
        gen(pair.first, pair.second);
      }
    }
  }
  
  void printFacts(){
    for (auto pair : vals){
      std::string s = "";
      s += pair.first->valString();
      s += " ";
      switch(pair.second.getType()){
        case INTVAL: 
          s += std::to_string(pair.second.intVal);
          break; 
        case CHARVAL:
          s += std::to_string(pair.second.charVal);
          break; 
        case BOOLVAL: 
          s += std::to_string(pair.second.boolVal);
          break; 
        default: 
          s += "TOPVAL";
          break; 
      }
      cout << s << endl;
    }
  }


  void gen(Opd * opd, ConstantVal v){
		auto itr = vals.find(opd);
		if (itr == vals.end()){
			vals[opd] = v;
		} else {
			ConstantVal cur = itr->second;
		}
	}

  bool updateVal(Opd * opd, ConstantVal v){
		auto itr = vals.find(opd);
		if (itr == vals.end()){
			cout << "updateVal: it's not in the list!\n";
			return false;
		} else {
      cout << "updateVal: it found me!\n";
			vals[opd] = v;
			return true;
		}
	}
	void kill(Opd * opd){
		vals.erase(opd);
	}
	//TODO: You'll probably want to add some extra convenience functions
	// to, for example, merge two constantsFacts objects, or determine if
	// a given set of values changes a fact set to determine if you've reached
	// saturation.

	ConstantVal getVal(Opd* opd){
		cout << "Getting Val of: " + opd->locString() << " ";
    std::map<Opd *, ConstantVal>::iterator it;
    it = vals.find(opd);
    if(it == vals.end()){
      cout << "didn't find it\n";
      ConstantVal v; 
      v.setTop();
      return v;
    } else {
      cout << it->second.valToString() << endl;
      return it->second;
    }
	}

private:
	std::map<Opd *, ConstantVal> vals;
};    

class ConstantsAnalysis{
public:
	static bool run(ControlFlowGraph * cfg){
		ConstantsAnalysis ca;
		return ca.runGraph(cfg);
	}

	ConstantsFacts* getInFacts(BasicBlock* bb)
	{
		std::map<BasicBlock*, ConstantsFacts>::iterator it;
		it = outFacts.find(bb);
		if(it == outFacts.end()){
		cout << "couldn't find in getInFacts\n";
		return nullptr;
		} else {
		return &(it->second);
    	}
	}
private:
	ConstantsAnalysis() : effectful(false){}
	bool runGraph(ControlFlowGraph * cfg); 
	bool runBlock(ControlFlowGraph * cfg, BasicBlock * block); 

	std::map<BasicBlock *, ConstantsFacts> outFacts;
	std::map<BasicBlock *, ConstantsFacts> inFacts;
	bool effectful;
};

}

#endif
