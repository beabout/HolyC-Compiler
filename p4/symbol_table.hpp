#ifndef HOLEYC_SYMBOL_TABLE_HPP
#define HOLEYC_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
#include <ostream>
#include "ast.hpp" // <-- Include if you need to access Nodes like 'TypeNode * myType;' 

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace holeyc{
  //A semantic symbol, which represents a single
  // variable, function, etc. Semantic symbols
  // exist for the lifetime of a scope in the
  // symbol table.
class SemSymbol{
  public: 
    SemSymbol(DeclNode* decl){};
    virtual IDNode* myID() = 0;
    virtual std::string myTypeToS() = 0;
};

class VarSymbol : public SemSymbol {
  public: 
    VarSymbol(VarDeclNode* decl)
    : SemSymbol(decl), myDecl(decl){};
    
    IDNode* myID() override{
      return myDecl->ID();
    }
    
    // a (Int)
    std::string myTypeToS() override{
      return this->myDecl->getTypeNode()->getMyString();
    }
  private: 
    VarDeclNode* myDecl;
};

class FnSymbol : public SemSymbol {
  public: 
    FnSymbol(FnDeclNode* decl)
    : SemSymbol(decl), myDecl(decl){};
    
    IDNode* myID() override {
      return myDecl->ID();
    }
    // fn (param1Type, param2Type, ..., paramNType->returnType)
    std::string myTypeToS() override{
      std::string s = "";
      int size = this->myDecl->getFormals()->size();
      int count = 0;
      for (auto vardecl : *this->myDecl->getFormals()){
        s += vardecl->getTypeNode()->getMyString();
        count++;
        if(count < size)
        {
          s += ",";
        }
        
      }
      s += " -> ";
      s += this->myDecl->getTypeNode()->getMyString();
      return s;
    }

  private:
    FnDeclNode* myDecl;
};

//A single scope. The symbol table is broken down into a
// chain of scope tables, and each scope table holds
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable
{
public:
  ScopeTable();
  //TODO: add functions for looking up symbols
  // and/or returning information to indicate
  // that the symbol does not exist within the
  // current scope.
  bool symbolPresent(IDNode* id);
  bool symAlreadyDefined(std::string key);
  void addSymbol(std::string name, SemSymbol * sym);

  private:
    HashMap<std::string, SemSymbol *> *symbols;
  };

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
    bool lookUp(std::string key); // Should take a key value and return true/false if variable/fn has already been declared.

    void dropScope(); // This will pop front once we have left a scope.

    void addScope(ScopeTable* s); // This will push new scope to front.
    ScopeTable * currentScope(); 
    bool isInScopeChain(IDNode* id);
    
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif
