%{
#include <string>
#include <limits.h>

/* Get our custom yyFlexScanner subclass */
#include "scanner.hpp"
#include "tokens.hpp"
#undef  YY_DECL
#define YY_DECL int holyc::Scanner::yylex( holyc::Parser::semantic_type * const lval )

/* define yyterminate as this instead of NULL */
#define yyterminate() return( TokenKind::END )

/* Exclude unistd.h for Visual Studio compatability. */
#define YY_NO_UNISTD_H

/* typedef to make the reference to token kinds shorter */
using TokenKind = holyc::Parser::token;

%}

%option debug
%option nodefault
%option yyclass="holyc::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

#* {}

"NULLPTR" {
  int tokenKind = TokenKind::NULLPTR;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 4;
  return tokenKind;
}

"TOCONSOLE" {
  int tokenKind = TokenKind::TOCONSOLE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 9;
  return tokenKind;
}

"FROMCONSOLE" {
  int tokenKind = TokenKind::FROMCONSOLE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 11;
  return tokenKind;
}

"true" {
  int tokenKind = TokenKind::TRUE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 4;
  return tokenKind;
}

"false" {
  int tokenKind = TokenKind::FALSE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 5;
  return tokenKind;
}

"if" {
  int tokenKind = TokenKind::IF;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}

"else" {
  int tokenKind = TokenKind::ELSE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 4;
  return tokenKind;
}

"while" {
  int tokenKind = TokenKind::WHILE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 5;
  return tokenKind;
}

"return" {
  int tokenKind = TokenKind::RETURN;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 6;
  return tokenKind;
}

"void" {
  int tokenKind = TokenKind::VOID;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 4;
  return tokenKind;
}

"charptr" {
  int tokenKind = TokenKind::CHARPTR;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 7;
  return tokenKind;
}

"char" {
  int tokenKind = TokenKind::CHAR;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 4;
  return tokenKind;
}

"boolptr" {
  int tokenKind = TokenKind::BOOLPTR;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 7;
  return tokenKind;
}

"bool" {
  int tokenKind = TokenKind::BOOL;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 4;
  return tokenKind;
}

"intptr" {
  int tokenKind = TokenKind::INTPTR;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 6;
  return tokenKind;
}

"int" {
  int tokenKind = TokenKind::INT;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 3;
  return tokenKind;
}

[\"](.*)([^\\]\")  {
  int tokenKind = TokenKind::STRLITERAL;
  this->yylval->tokenValue = new StrToken(lineNum, colNum, yytext);
  colNum += strlen(yytext);
  return tokenKind;
}

"||" {
  int tokenKind = TokenKind::OR;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}

"&&" {
  int tokenKind = TokenKind::AND;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}

"=" {
  int tokenKind = TokenKind::ASSIGN;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}

">=" {
  int tokenKind = TokenKind::GREATEREQ;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}
">" {
  int tokenKind = TokenKind::GREATER;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum++;
  return tokenKind;
}
"<=" {
  int tokenKind = TokenKind::LESSEQ;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}
"<" {
  int tokenKind = TokenKind::LESS;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}
"!=" {
  int tokenKind = TokenKind::NOTEQUALS;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}
"==" {
  int tokenKind = TokenKind::EQUALS;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum += 2;
  return tokenKind;
}
! {
  int tokenKind = TokenKind::NOT;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum++;
  return tokenKind;
}
[\*] {
    int tokenKind = TokenKind::STAR;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return tokenKind;
  }
[\/] {
    int tokenKind = TokenKind::SLASH;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return tokenKind;
    }
"++" {
    int tokenKind = TokenKind::CROSSCROSS;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum += 2;
    return tokenKind;
    }

"+"		{
		// Keep track of the "kind" of token we matched. In this case,
		// it's a CROSS (aka the plus symbol). The full list of token
		// kinds is declared around line 197 of grammar.hh. The testing
		// functionScanner::outputTokens also indicates what each token
		// represents (declared in scanner.cpp line 9)
		int tokenKind = TokenKind::CROSS;

		//Instatiate a token. There are 4 classes of tokens defined based on 
		// - a literal string (StringLitToken)
		// - a literal integer (IntLitToken)
		// - an identifier (IDToken)
		// - a base Token (any other type)
		// The token object is a handy place to keep information about the
		// token (i.e. token metadata) such as the line and column on which
		// the token character started (which can be used later in debug 
		// messages, etc). 
		Token * token = new Token(lineNum, colNum, tokenKind);

		//Assign the token to a instance variable of the scanner.
		// (When Flex processes a .l file in C++ mode, it creates 
		// a class called Scanner, which is a subclass of yyFlexScanner)
		// By assigning a value to this field, we put the token
		// metadata in a place where scanner's client program can find it.
		this->yylval->tokenValue = token;

		//Since we consumed 1 character, increase the column number by 1
		colNum++;

		//Let the scanner's client program know that a CROSS token
		// has been matched
		return tokenKind;
		}
"--" {
    int tokenKind = TokenKind::DASHDASH;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum += 2;
    return tokenKind;
    }
"-" {
		//Essentially the same thing as CROSS, but without the verbose
		// comments so it doesn't look so scary.
		int tokenKind = TokenKind::DASH;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
		}

{DIGIT}+	{
		//TODO: The following computation of the integer
		// value does not check for overflow. It should do so.
    int intVal = 0;
    try
    {
      intVal = std::stoi(yytext);
    } catch(const std::out_of_range& oor)
    {
      std::cout << "WARN [" << lineNum << "," << colNum << "]: Integer literal too large; using max value" << std::endl;
      intVal = INT_MAX;
    }



		//Note that the IntLitToken does not take a TokenKind argument
		// since it's implicitly known that the token represents an
		// INTLITERAL		
		yylval->tokenValue = new IntLitToken(lineNum, colNum, intVal);

		//TODO: update the colNum count according to the number of
		// characters consumed. 
    
    colNum += strlen(yytext);

		//As before, alert the client program that an INTLITERAL 
		// token was matched
		return TokenKind::INTLITERAL;
		}

\n		{
		//Update the line number, reset the columnNumber
		lineNum++;
		colNum = 0;
		}

[ \t]       {colNum += strlen(yytext);}

@ {
    int tokenKind = TokenKind::AT;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return TokenKind::AT;
  }
  
; {
  int tokenKind = TokenKind::SEMICOLON;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum++;
  return TokenKind::SEMICOLON;
}

, {
  int tokenKind = TokenKind::COMMA;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum++;
  return TokenKind::COMMA;
}
  
[\(] {
    int tokenKind = TokenKind::LPAREN;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return TokenKind::LPAREN;
    }
[\)] {
    int tokenKind = TokenKind::RPAREN;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return TokenKind::RPAREN;
    }
[\{] {
    int tokenKind = TokenKind::LCURLY;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return TokenKind::LCURLY;
    }
    
[\}] {
      int tokenKind = TokenKind::RCURLY;
      this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
      colNum++;
      return TokenKind::RCURLY;
      }
[\[] {
  int tokenKind = TokenKind::LBRACE;
  this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
  colNum++;
  return TokenKind::LBRACE;
  }
  
[\]] {
    int tokenKind = TokenKind::RBRACE;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return TokenKind::RBRACE;
    }

[\^] {
    int tokenKind = TokenKind::CARAT;
    this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
    colNum++;
    return TokenKind::CARAT;
  }
  
.   {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for. 
    std::cerr << "FATAL [" << std::to_string(lineNum) << "," << std::to_string(colNum) << "]: Illegal character " << yytext << std::endl;
		colNum += yyleng;
    }
%%
