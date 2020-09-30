#ifndef HOLEYC_SYMBOL_TABLE_HPP
#define HOLEYC_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>

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
    SemSymbol(char k, std::string t) {
      myKind = k;
      myType = t;
    }
    
    char kind() {
      return myKind; 
    }

    std::string type() {
      return myType;
    }
    
  private: 
    char myKind;
    std::string myType;
};

//A single scope. The symbol table is broken down into a 
// chain of scope tables, and each scope table holds 
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		//TODO: add functions for looking up symbols
		// and/or returning information to indicate
		// that the symbol does not exist within the
		// current scope.
    bool symbolPresent(std::string t) { 
      HashMap<std::string, SemSymbol *>::iterator it; 
      return symbols->contains(t);
    }

    void addSymbol(char k, std::string t) {
      SemSymbol * symbol = new SemSymbol(k, t);
      std::pair<std::string, SemSymbol *> pair = std::pair<std::string, SemSymbol *>(t, symbol);
      symbols->insert(pair);
    }

  private:
		HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
    void dropScope();
    void addScope();
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif
