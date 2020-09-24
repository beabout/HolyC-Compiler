#ifndef HOLEYC_AST_HPP
#define HOLEYC_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"

// **********************************************************************
// ASTnode class (base class for all other kinds of nodes)
// **********************************************************************

namespace holeyc{

/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/

class AndNode;
class AssignExpNode;
class AssignStmtNode;

class BoolTypeNode;
class BinaryExpNode;

class CallExpNode;
class CallStmtNode;
class CharLitNode;
class CharTypeNode;

class DerefNode;
class DeclNode;
class DeclListNode;
class DivideNode;

class ExpNode;
class EqualsNode;
class TrueNode;
class FalseNode;
class FromConsoleStmtNode;
class FnDeclNode;

class GreaterEqNode;
class GreaterNode;

class IDNode;
class IfElseStmtNode;
class IfStmtNode;
class IntLitNode;
class IndexNode;
class IntTypeNode;

class LessNode;
class LValNode;
class LessEqNode;

class MinusNode;

class NullPtrNode;
class NegNode;
class NotNode;
class NotEqualsNode;

class OrNode;

class PlusNode;
class ProgramNode;
class PostDecStmtNode;
class PostIncStmtNode;

class ReturnStmtNode;
class RefNode;

class StmtNode;
class StrLitNode;

class TypeNode;
class ToConsoleStmtNode;
class TimesNode;
class TrueNode;

class UnaryExpNode;

class VoidTypeNode;
class VarDeclNode;

class WhileStmtNode;

class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
	: l(lineIn), c(colIn){
	}
	virtual void unparse(std::ostream& out, int indent) = 0;
	size_t line(){ return l; }
	size_t col() { return c; }

	/**
	* Return a string specifying the position this node begins.
	* For some node a position doesn't really make sense (i.e.
	* ProgramNode) but for the rest it's the position in the 
	* input file that represents that node
	**/
	std::string pos(){
		return "[" + std::to_string(line()) + ","
		+ std::to_string(col()) + "]";
	}

private:
	size_t l; /// The line at which the node starts in the input file
	size_t c; /// The column at which the node starts in the input file
};

/************************************
 * AST NODES
 ************************************/

class ExpNode : public ASTNode {
  public:
    ExpNode(size_t line, size_t col)
    : ASTNode(line, col){
    }
    void unparse(std::ostream& out, int indent) override = 0;

};

class NestedExpNode : public ExpNode {
  public: 
    NestedExpNode(size_t line, size_t col, ExpNode* exp)
    : ExpNode(line, col), myExp(exp){
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
   ExpNode* myExp;
}


class TypeNode : public ASTNode {
  protected:
    TypeNode(size_t lineIn, size_t colIn, bool refIn)
        : ASTNode(lineIn, colIn), myIsReference(refIn)
    {
    }

  public:
    virtual void unparse(std::ostream &out, int indent) = 0;
    //TODO: consider adding an isRef to use in unparse to
    // indicate if this is a reference type
  private:
    bool myIsReference;
};

/** 
* \class ProgramNode
* Class that contains the entire abstract syntax tree for a program.
* Note the list of declarations encompasses all global declarations
* which includes (obviously) all global variables and struct declarations
* and (perhaps less obviously), all function declarations
**/
class ProgramNode : public ASTNode{
  public:
    ProgramNode(std::list<DeclNode *> * globalsIn) 
    : ASTNode(1, 1), myGlobals(globalsIn){
    }
    void unparse(std::ostream& out, int indent) override;
  private:
    std::list<DeclNode * > * myGlobals;
};

class StmtNode : public ASTNode{
  public: 
    StmtNode(size_t line, size_t col) 
    : ASTNode(line, col) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  //private: 
    size_t line;
    size_t col;
};

/************************************
 * STATEMENT NODES
 ************************************/

class LValNode : public ExpNode
{
  public:
      LValNode(size_t line, size_t col) 
    : ExpNode(line, col) {
    }
  private:
  
};

class DerefNode : public LValNode
{
  public:
      DerefNode(size_t line, size_t col) 
    : LValNode(line, col) {
    }
  private:
  IDNode* myIDNode;
};

class IndexNode : public LValNode
{
  public:
      IndexNode(size_t line, size_t col) 
    : LValNode(line, col) {
    }
  private:
      IDNode* myIDNode;
      ExpNode* offsetExpression;
};

class RefNode : public LValNode
{
  public:
      RefNode(size_t line, size_t col) 
    : LValNode(line, col) {
    }
  private:
    IDNode* myIDNode;
};

class AssignExpNode : public ExpNode
{
  public: 
    AssignExpNode(size_t line, size_t col, LValNode *lval, ExpNode *exp)
    : ExpNode(line,col), myLval(lval), myExp(exp) 
    {
    }

    void unparse(std::ostream &out, int indent);

  private: 
    LValNode *myLval;
    ExpNode *myExp;
};

class AssignStmtNode : public StmtNode{
  public:
    AssignStmtNode(size_t line, size_t col, AssignExpNode *assignExp) 
    : StmtNode(line, col), myAssignExp(assignExp) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
    AssignExpNode *myAssignExp;
};



class CallStmtNode : public StmtNode{
  public:
    CallStmtNode(size_t line, size_t col, ExpNode *callExp) 
    : StmtNode(line, col), myCallExp(callExp) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    ExpNode *myCallExp;
};



class FromConsoleStmtNode : public StmtNode{
  public:
    FromConsoleStmtNode(size_t line, size_t col, LValNode *lVal) 
    : StmtNode(line, col), myLVal(lVal) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    LValNode *myLVal;
};

class IfElseStmtNode : public StmtNode{
  public:
    IfElseStmtNode(size_t line, size_t col, ExpNode *exp, std::list<StmtNode *> *trueStmtNodes, std::list<StmtNode *> *falseStmtNodes)
        : StmtNode(line, col), myExp(exp), myTrueStmts(trueStmtNodes), myFalseStmts(falseStmtNodes)
    {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
    ExpNode *myExp;
    std::list<StmtNode *> *myTrueStmts;
    std::list<StmtNode *> *myFalseStmts;
};

class IfStmtNode : public StmtNode{
  public:
    IfStmtNode(size_t line, size_t col, ExpNode *exp, std::list<StmtNode *> *stmtNodes) 
    : StmtNode(line, col), myExp(exp), myStmts(stmtNodes) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
    ExpNode *myExp;
    std::list<StmtNode *> *myStmts;
};

class PostDecStmtNode : public StmtNode{
  public:
    PostDecStmtNode(size_t line, size_t col, LValNode *loc)
        : StmtNode(line, col), myLoc(loc) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    LValNode *myLoc;
};

class PostIncStmtNode : public StmtNode{
  public:
    PostIncStmtNode(size_t line, size_t col, LValNode *loc)
        : StmtNode(line, col), myLoc(loc) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    LValNode *myLoc;
};

/* CLB - ReturnStmtNode can either HAVE an expNode or not. 
 * Therefore, in the ReturnStmt unparse method, we'll have to check for that. 
 */
class ReturnStmtNode : public StmtNode{
  public:
    ReturnStmtNode(size_t line, size_t col, ExpNode *exp) 
    : StmtNode(line, col), myExp(exp) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
    ExpNode *myExp;
};

class ToConsoleStmtNode : public StmtNode{
  public:
    ToConsoleStmtNode(size_t line, size_t col, ExpNode *exp) 
    : StmtNode(line, col), myExp(exp) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
    ExpNode *myExp;
};

class WhileStmtNode : public StmtNode {
  public:
    WhileStmtNode(size_t line, size_t col, ExpNode *exp, std::list<StmtNode *> *stmtNodes) 
    : StmtNode(line, col), myExp(exp), myStmts(stmtNodes)  {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private:
    ExpNode *myExp;
    std::list<StmtNode *> *myStmts;
};

class DeclNode : public StmtNode {
  public:
    DeclNode(size_t line, size_t col)
        : StmtNode(line, col) {
    }
    void unparse(std::ostream &out, int indent) override = 0;
  private:
};

/************************************
 * EXPRESSION NODES
 ************************************/

/**  \class ExpNode
* Superclass for expression nodes (i.e. nodes that can be used as
* part of an expression).  Nodes that are part of an expression
* should inherit from this abstract superclass.
**/



class BinaryExpNode : public ExpNode
{
    public:
      BinaryExpNode(size_t line, size_t col, ExpNode *left, ExpNode *right)
      : ExpNode(line, col), lhs(left), rhs(right) {
      }
    private:
     ExpNode* lhs;
     ExpNode* rhs;
};

	/************************************
 	* BINARY EXPRESSION NODES
 	************************************/
 	class AndNode : public BinaryExpNode
	{
      public:
        AndNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
        : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
        }
        void unparse(std::ostream& out, int indent);
      private:
        ExpNode* myLHS;
        ExpNode* myRHS;
	};

	class DivideNode : public BinaryExpNode
	{
    public:
      DivideNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class EqualsNode : public BinaryExpNode
	{
    public:
      EqualsNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class GreaterEqNode : public BinaryExpNode
	{
    public:
      GreaterEqNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class GreaterNode : public BinaryExpNode
	{
    public:
      GreaterNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class LessEqNode : public BinaryExpNode
	{
    public:
      LessEqNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class LessNode : public BinaryExpNode
	{
    public:
      LessNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};


	class NotEqualsNode : public BinaryExpNode
	{
    public:
      NotEqualsNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class OrNode : public BinaryExpNode
	{
    public:
      OrNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class PlusNode : public BinaryExpNode
	{
    public:
      PlusNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line,col,lhs,rhs), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};

	class TimesNode : public BinaryExpNode
	{
    public:
      TimesNode(size_t line, size_t col, ExpNode* lhs, ExpNode* rhs)
      : BinaryExpNode(line, col), myLHS(lhs), myRHS(rhs){
      }
      void unparse(std::ostream& out, int indent);
    private:
      ExpNode* myLHS;
      ExpNode* myRHS;
	};


	class MinusNode : public BinaryExpNode
	{
    public:
    MinusNode(size_t line, size_t col)
    : BinaryExpNode(line, col){
    }
    void unparse(std::ostream& out, int indent);
	};

class CharLitNode : public ExpNode
{
  public: 
    CharLitNode(CharLitToken * token)
    : ExpNode(token->line(), token->col()), myChar(token->val()){
    }
    void unparse(std::ostream& out, int indent);
    private: 
      char myChar;
};



class IntLitNode : public ExpNode
{
  public:
    IntLitNode(IntLitToken *token)
        : ExpNode(token->line(), token->col()), myInt(token->num())
    {
    }
    void unparse(std::ostream& out, int indent);
    private: 
      int myInt;
};

class CallExpNode : public ExpNode 
{
  public: 
    CallExpNode(IDToken * token)
    : ExpNode(token->line(), token->col()), myStrVal(token->value()){

    }
    CallExpNode(IDToken *token, std::list<ExpNode*>* exp_list)
        : ExpNode(token->line(), token->col()), myStrVal(token->value()), myExps(exp_list)
    {

    }
    void unparse(std::ostream& out, int indent);
    private: 
      std::string myStrVal;
      std::list<ExpNode *> *myExps;
};



/************************************
* LVAL NODES
************************************/
class DerefNode : public LValNode
{	

};

// TODO: THIS WAS ALSO GIVEN BUT WITH WRONG INHERITANCE????
/** An identifier. Note that IDNodes subclass
 * ExpNode because they can be used as part of an expression. 
**/
class IDNode : public ExpNode{
public:
  IDNode(IDToken *token, bool isAt, bool isCarat)
      : ExpNode(token->line(), token->col()), myStrVal(token->value()), myIsAt(isAt), myIsCarat(isCarat))
  {
  }
  void unparse(std::ostream& out, int indent);
private:
  /** The name of the identifier **/
  std::string myStrVal;
  bool myIsAt;
  bool myIsCarat;
};

class IndexNode : public LValNode
{	

};

class RefNode : public LValNode
{	

};

class NullPtrNode : public ExpNode
{

};

class StrLitNode : public ExpNode
{

};

class FalseNode : public ExpNode
{
  public:
  FalseNode(size_t lineIn, size_t colIn, bool bool_param)
    : ExpNode(token->line(), token->col()), myBool(token->value())
  {
  }
  void unparse(std::ostream &out, int indent);
  private:
  bool myBool;
};

class TrueNode : public ExpNode
{
  public:
    TrueNode(size_t lineIn, size_t colIn, bool bool_param)
      : ExpNode(token->line(), token->col()), myBool(token->value())
    {
    }
    void unparse(std::ostream &out, int indent);
  private:
    bool myBool;
};

class UnaryExpNode : public ExpNode
{
  public: 
    UnaryExpNode(ExpNode* exp)
    : ExpNode(exp->line(), exp->col()), myExp(exp){
    }
    void unparse(std::ostream &out, int indent);
  private:
    ExpNode* myExp;
};

	/************************************
 	* UNARY NODES
 	************************************/
 	class NegNode : public UnaryExpNode
	{
    public: 
      NegNode(ExpNode* exp)
      : UnaryExpNode(exp), myExp(exp){
      }
      void unparse(std::ostream &out, int indent);
    private: 
      ExpNode* myExp;
	};

	class NotNode : public UnaryExpNode
	{
    public: 
      NotNode(ExpNode* exp)
      : UnaryExpNode(exp), myExp(exp){
      }
      void unparse(std::ostream &out, int indent);
    private: 
      ExpNode* myExp;
	};


/*****************************
 * TYPE NODES
 ****************************/
/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in 
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/

  class BoolTypeNode : public TypeNode{
    public:
      BoolTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
        : TypeNode(lineIn, colIn, isRefIn)
      {
      }
      void unparse(std::ostream &out, int indent);
    private: 
  };

  class CharTypeNode : public TypeNode{
    public:
      CharTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
        : TypeNode(lineIn, colIn, isRefIn)
      {
      }
      void unparse(std::ostream &out, int indent);
    private: 
  };

  class IntTypeNode : public TypeNode
  {
    public:
      IntTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
        : TypeNode(lineIn, colIn, isRefIn)
      {
      }
      void unparse(std::ostream &out, int indent);
    private:
  };

  class VoidTypeNode : public TypeNode{
    public: 
      VoidTypeNode(size_t lineIn, size_t colIn, bool isRefIn) 
      : TypeNode(lineIn, colIn, isRefIn)
      {
      }
      void unparse(std::ostream &out, int indent);
    private:
  };

  /** A variable declaration. Note that this class is intended to 
 * represent a global or local variable of any type (including a struct
 * type. Note that this is not intended to represent a declaration of
 * a struct. In other words:
 * struct MyStruct { 
 *   int fieldA;
 * };
 * is NOT a VarDeclNode because it introduces a new datatype, not a new
 * variable (in this case, the example is a StructDeclNode).  * However,
 * struct MyStruct instance; *is* a VarDeclNode, since it introduces a 
 * new variable to the program. 
**/

  /************************************
 * DECLARATION NODES
 ************************************/

  /** \class DeclNode
* Superclass for declarations (i.e. nodes that can be used to 
* declare a struct, function, variable, etc).  This base class will 
**/

  class VarDeclNode : public DeclNode
  {
  public:
    VarDeclNode(size_t line, size_t col, TypeNode *type, IDNode *id)
        : DeclNode(type->line(), type->col()), myType(type), myId(id)
    {
    }
    void unparse(std::ostream &out, int indent);

    // private:
    TypeNode *myType;
    IDNode *myId;
};

class FnDeclNode : public DeclNode
{


  public:
    FnDeclNode(size_t line, size_t col, TypeNode *type, IDNode *id, std::list<VarDeclNode *> * decl_nodes, std::list<StmtNode *> * stmt_nodes)
        : DeclNode(type->line(), type->col()), myType(type), myId(id), formalDeclNodes(decl_nodes), stmtNodes(stmt_nodes)
    {
    }
    
    void unparse(std::ostream &out, int indent);

 // private: 
    TypeNode *myType;
    IDNode *myId;
    std::list<VarDeclNode *> *formalDeclNodes;
    std::list<StmtNode *> *stmtNodes;
};





} //End namespace holeyc

#endif