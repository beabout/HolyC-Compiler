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

  void SymbolTable::dropScope(){
  }
  
  void SymbolTable::addScope(ScopeTable * s){
    scopeTableChain->push_front(s);
  }

  ScopeTable * SymbolTable::currentScope() {
    return scopeTableChain->front();
  }

  bool SymbolTable::isInScopeChain(IDNode *id) {
    bool idIsPresent = false;
    std::string name = id->getName();
    for (auto scopeTable : *scopeTableChain){
      idIsPresent = scopeTable->symbolPresent(name);
      if(idIsPresent == true){
        return idIsPresent;
      }
    }
    return idIsPresent;
  }

  /*
  ********** Scope Table Methods **********
  */

  ScopeTable::ScopeTable()
  {
    symbols = new HashMap<std::string, SemSymbol *>();
  }

  bool ScopeTable::symbolPresent(std::string name)
  {
    return symbols->contains(name);
  }

  void ScopeTable::addSymbol(SemSymbol* sym)
  {
    // SemSymbol *symbol = new SemSymbol(k, t);
    // std::pair<std::string, SemSymbol *> pair = std::pair<std::string, SemSymbol *>(t, symbol);
    // symbols->insert(pair);
  }

  /*
  ********** Semantic Table Methods **********
  */
}