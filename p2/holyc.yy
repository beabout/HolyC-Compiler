%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace {holyc}
 /*
 If your bison install has trouble with the 
 line %define api.parser.class {Parser} try
 using the older %define parser_class_name {Parser}
 instead
 */
%define api.parser.class {Parser}
%define parse.assert
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	namespace holyc {
		class Scanner;
	}

//The following definition is required when 
// we don't use the %locations directive (which we won't)
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { holyc::Scanner &scanner }

%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   //#include "tokens.hpp"

  //Request tokens from our scanner member, not 
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}

/*
The %union directive is a way to specify the 
set of possible types that might be used as
translation attributes that a symbol might take.
For this project, only terminals have types (we'll
have translation attributes for non-terminals in the next
project)
*/
%union {
   holyc::Token*                         transToken;
}

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

/* NOTE: Make sure to add precedence and associativity 
 * declarations
*/

%left CROSS DASH // plus and minus 
%left STAR SLASH // mult and div 

// %type <transIntToken> exp
// %type <transIntToken> term


%%

/* TODO: add productions for the other nonterminals in the 
   grammar and make sure that all of the productions of the 
   given nonterminals are complete
*/

program 	      : globals { }

globals 	      : globals decl { }
                | globals functiondecl {}
		          | /* epsilon */ { }

decl     		    : varDecl SEMICOLON { } // int num; (OK outside functions)

varDecl 	      : type id { }

type          	: VOID 
                | INT
                | INTPTR  
                | BOOL
                | BOOLPTR
                | CHAR
                | CHARPTR

reassign : id ASSIGN exp SEMICOLON // x = 9 + 4;

exp : exp OR b | b
b : b AND c | c
c : c EQUALS d | c NOTEQUALS d | c GREATER d | c GREATEREQ d | c LESS d | c LESSEQ d | d
d : d CROSS e | d DASH e | e 
e : e STAR f | e SLASH f | f
f : DASH term | NOT term | term 


functiondecl : varDecl LPAREN paramlist RPAREN LCURLY functionbody RCURLY
paramlist : varDecl paramlist2 | /* epsilon */
paramlist2 : COMMA varDecl paramlist2 | /* epsilon */

functionbody : decl functionbody 
             | reassign functionbody 
             | whileblock functionbody 
             | ifblock functionbody 
             | RETURN SEMICOLON functionbody 
             | /* epsilon */

ifblock : IF LPAREN conditionexpr RPAREN LCURLY functionbody RCURLY

whileblock : WHILE LPAREN conditionexpr RPAREN LCURLY functionbody RCURLY

// if(a < (b<(c==d)))

// WHAT THIS ALLOWS: 
// (4 == b) > (4 > 5)
// 4 > (4 > 5)
// 4 > 5
// 4 > a 
conditionexpr : term conditional conditionexpr2 // 4 > (4 > 5), 4 > 5, 4 > a 
              | id conditional conditionexpr2 // num > (4 > 5), num > 5, num > a 
              | conditionexpr conditional conditionexpr2 
              
conditionexpr2 : LPAREN conditionalexpr RPAREN
               | term
               | id

conditionalexpr : LPAREN termorid conditional termorid RPAREN // (4 < d)
                | termorid conditional termorid // 4 < d
                | conditionalexpr conditional termorid // 4 < (d == 4)

                
termorid : term | id

conditional : GREATER 
            | EQUALS 
            | AND 
            | OR
            | NOTEQUALS 
            | GREATEREQ 
            | LESS 
            | LESSEQ

term            : lval
                | INTLITERAL
                | STRLITERAL
                | CHARLIT
                | TRUE
                | FALSE
                | NULLPTR
                | LPAREN exp RPAREN

lval            : id
                | id LBRACE exp RBRACE
                | AT id
                | CARAT id

id	          	: ID

%%

void holyc::Parser::error(const std::string& err_message){
   /* For project grading, only report "syntax error"
      if a program has bad syntax. However, you will
      probably want better output for debugging. Thus,
      this error function prints a verbose message to 
      stdout, but only prints "syntax error" to stderr
   */
	std::cout << err_message << std::endl;
	std::cerr << "syntax error" << std::endl;
}
