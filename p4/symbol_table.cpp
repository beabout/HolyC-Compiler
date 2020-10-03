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

  bool SymbolTable::lookUp(std::string key){
    return scopeTableChain->front()->symAlreadyDefined(key);
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
    for (auto scopeTable : *scopeTableChain){
      idIsPresent = scopeTable->symbolPresent(id);
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

  bool ScopeTable::symbolPresent(IDNode *id)
  {
    bool symbolIsPresent = false;
    for ( auto it = symbols->begin(); it != symbols->end(); ++it){
      if (it->first == id->getName()){
        id->SetSymbol(it->second);
        symbolIsPresent = true;
      }
    }
    return symbolIsPresent;
  }

  bool ScopeTable::symAlreadyDefined(std::string key){
    bool symbolIsPresent = false;
    for (auto it = symbols->begin(); it != symbols->end(); ++it){
      if (it->first == key){
        symbolIsPresent = true;
      }
    }
    return symbolIsPresent;
  }

  void ScopeTable::addSymbol(std::string name, SemSymbol* sym)
  {
     //SemSymbol *symbol = new SemSymbol(k, t);
     std::pair<std::string, SemSymbol *> pair = std::pair<std::string, SemSymbol *>(name, sym);
     symbols->insert(pair);
  }

  /*
  ********** Semantic Symbol Methods **********
  */
}