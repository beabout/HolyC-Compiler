%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace{holeyc}
%define api.parser.class {Parser}
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	#include "ast.hpp"
	namespace holeyc {
		class Scanner;
	}

//The following definition is required when 
// we don't have the %locations directive
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { holeyc::Scanner &scanner }
%parse-param { holeyc::ProgramNode** root }

%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   #include "ast.hpp"
   #include "tokens.hpp"

  //Request tokens from our scanner member, not 
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}


/*
The %union directive is a way to specify the 
set of possible types that might be used as
translation attributes in the syntax-directed
translations. 

TODO: You will have to add to this list to 
create new translation value types
*/
%union {
   holeyc::Token *                     transToken;
   holeyc::IDToken *                   transIDToken;
   holeyc::StrToken *                  transStrToken;
   holeyc::CharLitToken *              transCharToken;
   holeyc::IntLitToken *               transIntToken;
   holeyc::ProgramNode *               transProgram;
   std::list<holeyc::DeclNode *> *     transDeclList;
   holeyc::DeclNode *                  transDecl;
   holeyc::VarDeclNode *               transVarDecl;
   holeyc::FnDeclNode *                transFnDecl;
   holeyc::TypeNode *                  transType;
   holeyc::IDNode *                    transID;     
   holeyc::ExpNode *                   transExp;
   std::list<holeyc::StmtNode *> *     transStmtList;
   holeyc::StmtNode *                  transStmt;
   std::list<holeyc::VarDeclNode *> *  transVarDeclList;
   holeyc::LValNode *                  transLVal;
   holeyc::CallExpNode *               transCallExp;
   std::list<holeyc::ExpNode *> *      transExpList;
   holeyc::AssignExpNode *             transAssignExp;
}

%define parse.assert

%token                   END	   0 "end file"
%token	<transToken>     AND
%token	<transToken>     AT
%token	<transToken>     ASSIGN
%token	<transToken>     BOOL
%token	<transToken>     BOOLPTR
%token	<transToken>     CARAT
%token	<transToken>     CHAR
%token	<transCharToken> CHARLIT
%token	<transToken>     CHARPTR
%token	<transToken>     COMMA
%token	<transToken>     CROSS
%token	<transToken>     CROSSCROSS
%token	<transToken>     DASH
%token	<transToken>     DASHDASH
%token	<transToken>     ELSE
%token	<transToken>     EQUALS
%token	<transToken>     FALSE
%token	<transToken>     FROMCONSOLE
%token	<transIDToken>   ID
%token	<transToken>     IF
%token	<transToken>     INT
%token	<transIntToken>  INTLITERAL
%token	<transToken>     INTPTR
%token	<transToken>     GREATER
%token	<transToken>     GREATEREQ
%token	<transToken>     LBRACE
%token	<transToken>     LCURLY
%token	<transToken>     LESS
%token	<transToken>     LESSEQ
%token	<transToken>     LPAREN
%token	<transToken>     NOT
%token	<transToken>     NOTEQUALS
%token	<transToken>     NULLPTR
%token	<transToken>     OR
%token	<transToken>     RBRACE
%token	<transToken>     RCURLY
%token	<transToken>     RETURN
%token	<transToken>     RPAREN
%token	<transToken>     SEMICOLON
%token	<transToken>     SLASH
%token	<transToken>     STAR
%token	<transStrToken>  STRLITERAL
%token	<transToken>     TOCONSOLE
%token	<transToken>     TRUE
%token	<transToken>     VOID
%token	<transToken>     WHILE


/* Nonterminals
*  TODO: You will need to add more nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type). Note that the specifier in brackets
*  indicates the type of the translation attribute using
*  the names defined in the %union directive above
*/
/*    (attribute type)    (nonterminal)    */
%type <transProgram>      program
%type <transDeclList>     globals
%type <transDecl>         decl
%type <transVarDecl>      varDecl
%type <transFnDecl>       fnDecl
%type <transType>         type
%type <transID>           id
%type <transExp>          term
%type <transExp>          exp
%type <transStmtList>     fnBody
%type <transVarDeclList>  formals
%type <transVarDeclList>  formalsList
%type <transVarDecl>      formalDecl
%type <transLVal>          lval
%type <transStmt>         stmt
%type <transStmtList>     stmtList
%type <transAssignExp>    assignExp
%type <transCallExp>      callExp
%type <transExpList>      expList
// %type <transSomething>    actualsList

// Really just making SDT for nonterminals to generate AST

%right ASSIGN
%left OR
%left AND
%nonassoc LESS GREATER LESSEQ GREATEREQ EQUALS NOTEQUALS
%left DASH CROSS
%left STAR SLASH
%left NOT 
%left AT CARAT

%%

program 	: globals
		  {
		  $$ = new ProgramNode($1);
		  *root = $$;
		  }

globals 	: globals decl 
		  {
        $$ = $1;
        DeclNode * aGlobalDecl = $2;
        $1->push_back(aGlobalDecl);
      }
		  | /* epsilon */
		  {
        std::list<DeclNode *> * startingGlobals;
        startingGlobals = new std::list<DeclNode *>();
        $$ = startingGlobals;
		  }

decl 		: varDecl SEMICOLON
		  {
        //TODO: Make sure to fill out this rule
        // (as well as any other empty rule!)
        $$ = $1;
		  }
		| fnDecl 
		  { 
        //size_t tLine = $1->line;
        // size_t tCol = $1->col;
        $$ = $1;
      }

varDecl 	: type id
		  { 
        size_t typeLine = $1->line();
        size_t typeCol = $1->col();
        $$ = new VarDeclNode(typeLine, typeCol, $1, $2);
		  }

type : INT
		  {
        // the third parameter marks whether the type is a pointer type or not.
        $$ = new IntTypeNode($1->line(), $1->col(), false); 
		  }
		| INTPTR
		  { 
        $$ = new IntTypeNode($1->line(), $1->col(), true); 
      }
		| BOOL
		  { 
        $$ = new BoolTypeNode($1->line(), $1->col(), false); 
      }
		| BOOLPTR
		  { 
        $$ = new BoolTypeNode($1->line(), $1->col(), true); 
      }
		| CHAR
		  { 
        $$ = new CharTypeNode($1->line(), $1->col(), false); 
      }
		| CHARPTR
		  { 
        $$ = new CharTypeNode($1->line(), $1->col(), true); 
      }
		| VOID
		  { 
        $$ = new VoidTypeNode($1->line(), $1->col(), false); 
      }

fnDecl 		: type id formals fnBody
		  { 
        size_t tLine = $1->line();
        size_t tCol = $1->col();
        $$ = new FnDeclNode(tLine, tCol, $1, $2, $3, $4);
      }

formals 	: LPAREN RPAREN
		  { 
			  $$ = new std::list<VarDeclNode*>(); // ERRORS
		  }
		| LPAREN formalsList RPAREN
		  { 
			  $$ = $2;
		  }


formalsList	: formalDecl
		  { 
			  std::list<VarDeclNode*>* result = new std::list<VarDeclNode*>();
			  result->push_back($1);
			  $$ = result;
		  }
		| formalDecl COMMA formalsList 
		  { 
			  std::list<VarDeclNode*>* result = new std::list<VarDeclNode*>();
			  result->merge($1);
			  result->merge($3);
			  $$ = result;
		  }

formalDecl 	: type id
		  { 
        size_t tLine = $1->line();
        size_t tCol = $1->col();
        $$ = new VarDeclNode(tLine, tCol, $1, $2);
		  }

fnBody		: LCURLY stmtList RCURLY
		  { 
			  $$ = $2;
		  }

stmtList 	: /* epsilon */
		  { 
			  $$ = new std::list<StmtNode *>();
		  }
		| stmtList stmt
		  {
        StmtNode * stmt = $2;
        std::list<StmtNode*>* lst = new std::list<StmtNode*>();
        lst->push_back(stmt);
			  $1->merge(lst);
			  $$ = $1;
		   }

stmt		: varDecl SEMICOLON
		  { 
			  $$ = $1;
		  }
		| assignExp SEMICOLON
		  { 
        AssignStmtNode * assign = new AssignStmtNode($1->line(), $1->col(), $1);
			  $$ = assign;
		  }
		| lval DASHDASH SEMICOLON
		  { 
        size_t tLine = $1->line();
        size_t tCol = $1->col();
			  $$ = new PostDecStmtNode(tLine, tCol, $1);
		  }
		| lval CROSSCROSS SEMICOLON
		  { 
        size_t tLine = $1->line();
        size_t tCol = $1->col();
			 $$ = new PostIncStmtNode(tLine, tCol, $1); 
		  }
		| FROMCONSOLE lval SEMICOLON
		  { 
			  $$ = new FromConsoleStmtNode($2);
		  }
		| TOCONSOLE exp SEMICOLON
		  {
        size_t tLine = $1->line();
        size_t tCol = $1->col();
			  $$ = new ToConsoleStmtNode(tLine, tCol, $2);
			}
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { 
        size_t tLine = $3->line();
        size_t tCol = $3->col();
			  $$ = new IfStmtNode(tLine, tCol, $3, $6);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
		  { 
        size_t tLine = $3->line();
        size_t tCol = $3->col();
			  $$ = new IfElseStmtNode(tLine, tCol, $3, $6, $10);
		  }
		| WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { 
        size_t tLine = $3->line();
        size_t tCol = $3->col();
        $$ = new WhileStmtNode(tLine, tCol, $3, $6);
      }
		| RETURN exp SEMICOLON
		  { 
        size_t tLine = $2->line();
        size_t tCol = $2->col();
			  $$ = new ReturnStmtNode(tLine, tCol, $2);
      }
		| RETURN SEMICOLON
		  { 
        size_t tLine = $1->line();
        size_t tCol = $1->col();
        $$ = new ReturnStmtNode(tLine, tCol, nullptr);
      }
		| callExp SEMICOLON
		  { 
        CallStmtNode * call = new CallStmtNode($1->line(), $1->col(), $1);
        $$ = call;
      }

exp		: assignExp 
		  { 
			  $$ = $1;
		  }
		| exp DASH exp
		  { 
			  $$ = new MinusNode($1->line(), $1->col(),$1,$3);
		  }
		| exp CROSS exp
		  { 
        $$ = new PlusNode($1->line(), $1->col(),$1,$3);
      }
		| exp STAR exp
		  { 
        $$ = new TimesNode($1->line(), $1->col(),$1,$3);
      }
		| exp SLASH exp
		  { 
        $$ = new DivideNode($1->line(), $1->col(),$1,$3);
      }
		| exp AND exp
		  { 
        $$ = new AndNode($1->line(), $1->col(),$1,$3);
      }
		| exp OR exp
		  { 
        $$ = new OrNode($1->line(), $1->col(),$1,$3);
      }
		| exp EQUALS exp
		  { 
        $$ = new EqualsNode($1->line(), $1->col(),$1,$3);
      }
		| exp NOTEQUALS exp
		  { 
        $$ = new NotEqualsNode($1->line(), $1->col(),$1,$3);
      }
		| exp GREATER exp
		  { 
        $$ = new GreaterNode($1->line(), $1->col(),$1,$3);
      }
		| exp GREATEREQ exp
		  { 
        $$ = new GreaterEqNode($1->line(), $1->col(),$1,$3);
      }
		| exp LESS exp
		  { 
        $$ = new LessNode($1->line(), $1->col(),$1,$3);
      }
		| exp LESSEQ exp
		  { 
        $$ = new LessEqNode($1->line(), $1->col(),$1,$3);
      }
		| NOT exp
		  { 
        $$ = new NotNode($2);
      }
		| DASH term
		  { 
        $$ = new NegNode($2);
      }
		| term 
		  { 
        $$ = $1;
      }

assignExp	: lval ASSIGN exp
		  { 
        $$ = new AssignExpNode($1->line(), $1->col(), $1, $3);
      }

callExp		: id LPAREN RPAREN
		  { 
        std::list<ExpNode*>* emptylist = new std::list<ExpNode*>();
        $$ = new CallExpNode($1, emptylist);
      }
		// | id LPAREN actualsList RPAREN
		//   { 
		// 	  $$ = new CallExpNode($1, $3) // $1 -> id, $3 -> list of expressions 
    //   }

// actualsList	: exp
// 		  { 
// 			  std::list<ExpNode*>* result = new std::list<ExpNode*>();
// 			  result.push_back($1);
// 			  $$ = result;
//       }
// 		| actualsList COMMA exp
// 		  { 
//         // list magic 
// 			  std::list<ExpNode*>* result = new std::list<ExpNode*>();
// 			  result.push_back($1);
// 			  $1->merge(result);
// 			  $$ = $1;
//       }

term : lval
      { 
        $$ = $1;
      }
    | callExp
      { 
        $$ = $1;
      }

    | NULLPTR
		  { 
        Token * t = $1;
        $$ = new NullPtrNode(t->line(),t->col());
      }
		| INTLITERAL 
		  { 
        IntLitToken * t = $1;
        $$ = new IntLitNode(t);
      }
		| STRLITERAL 
		  { 
        StrToken * t = $1;
        $$ = new StrLitNode(t);
      }
		| CHARLIT 
		  { 
        CharLitToken * t = $1;
        $$ = new CharLitNode(t);
      }
		| TRUE
		  {         
        $$ = new TrueNode($1->line(), $1->col());
      }
		| FALSE
		  {         
        $$ = new FalseNode($1->line(), $1->col());
      }
		| LPAREN exp RPAREN
		  { 
        $$ = new NestedExpNode($1->line(),$1->col(), $2);
      }

lval		: id
		  {
        $$ = $1;
		  }
		| id LBRACE exp RBRACE
		  {
			  $$ = new IndexNode($1->line(), $1->col(), $1, $3);
		  }
		| AT id
		  {
        $2->myIsAt = true;
		    $$ = $2; // $2 here is already an IDNode
		  }
		| CARAT id
		  {
        $2->myIsCarat = true;
		    $$ = $2; // How to pass isCarat?
		  }

id		: ID
		  {
        size_t tLine = $1->line();
        size_t tCol = $1->col();
        IDToken * t = $1;
		    $$ = new IDNode(t,false,false);
		  }

%%

void holeyc::Parser::error(const std::string& msg){
	std::cout << msg << std::endl;
	std::cerr << "syntax error" << std::endl;
}
