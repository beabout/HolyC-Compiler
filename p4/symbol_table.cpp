#include "symbol_table.hpp"
namespace holeyc{
  /*
  ********** Symbol Table Methods **********
  */

  SymbolTable::SymbolTable(){
    //TODO: implement the list of hashtables approach
    // to building a symbol table:
    // Upon entry to a scope a new scope table will be 
    // entered into the front of the chain and upon exit the 
    // latest scope table will be removed from the front of 
    // the chain.
    scopeTableChain = new std::list<ScopeTable *>();
  }

  void SymbolTable::dropScope()
  {
  }
  
  void SymbolTable::addScope()
  {
  }

  /*
  ********** Scope Table Methods **********
  */

  ScopeTable::ScopeTable()
  {
    symbols = new HashMap<std::string, SemSymbol *>();
  }

  bool ScopeTable::symbolPresent(std::string t)
  {
    HashMap<std::string, SemSymbol *>::iterator it;
    return symbols->contain(t);
  }

  void ScopeTable::addSymbol(char k, std::string t)
  {
    SemSymbol *symbol = new SemSymbol(k, t);
    std::pair<std::string, SemSymbol *> pair = std::pair<std::string, SemSymbol *>(t, symbol);
    symbols->insert(pair);
  }

  /*
  ********** Semantic Table Methods **********
  */

  SemSymbol::SemSymbol(char k, std::string t){
    myKind = k;
    myType = t;
  }

  char SemSymbol::kind(){
    return myKind;
  }

  std::string SemSymbol::type(){
    return myType;
  }
}
