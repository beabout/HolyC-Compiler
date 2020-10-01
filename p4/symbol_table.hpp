#ifndef HOLEYC_SYMBOL_TABLE_HPP
#define HOLEYC_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
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
class SemSymbol {
	//TODO add the fields that 
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields
  public: 
    /* params:
    * k, the kind of symbol (either 'v' for variable or 'f' for function)
    * t, the (variable type) or (function return type) of symbol (either 'int', 'char', 'void', 'std::string', etc)
    */
    SemSymbol(char kind, DeclNode* decl){
      myDecl = decl;
      myKind = kind;
    };
    virtual std::string myIDToS();
    virtual std::string myTypeToS();

  private: 
    DeclNode* myDecl; 
    char myKind;

    // Consider having a TypeNode * ?
};

class VarSymbol : public SemSymbol {
  public: 
    VarSymbol(VarDeclNode* decl)
    : SemSymbol('v',decl), myDecl(decl){};
    
    std::string myIDToS(){
      return myDecl->ID()->getName();
    }
    
    std::string myTypeToS(){
      return myDecl->getTypeNode()->getMyString();
    }
  private: 
    VarDeclNode* myDecl;
};

class FnSymbol : public SemSymbol {
  public: 
    FnSymbol(FnDeclNode* decl)
    : SemSymbol('f',decl), myDecl(decl){};
    
    std::string myIDToS(){
      return myDecl->ID()->getName();
    }

    std::string myTypeToS(){
      return myDecl->getTypeNode()->getMyString();
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
  bool symbolPresent(std::string t);
  void addSymbol(SemSymbol* sym);

private:
  HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
    bool lookUp(); // Should take a key value and return true/false if variable/fn has already been declared.

    void dropScope(); // This will pop front once we have left a scope.

    void addScope(ScopeTable* s); // This will push new scope to front.
    ScopeTable * currentScope(); 
    bool isInScopeChain(IDNode* id);
    
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif
