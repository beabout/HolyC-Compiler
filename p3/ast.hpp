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
class DeclListNode;
class DeclNode;
class TypeNode;
class IDNode;

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
  protected:
    ExpNode(size_t line, size_t col)
    : ASTNode(line, col)
    {
    }
};

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
  private: 
    size_t line;
    size_t col;
};

/************************************
 * STATEMENT NODES
 ************************************/

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
    CallStmtNode(size_t line, size_t col, CallExpNode *callExp) 
    : StmtNode(line, col), myCallExp(callExp) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    CallExpNode *myCallExp;
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
    PostDecStmtNode(size_t line, size_t col, LocNode *loc)
        : StmtNode(line, col), myLoc(loc) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    LocNode *myLoc;
};

class PostIncStmtNode : public StmtNode{
  public:
    PostIncStmtNode(size_t line, size_t col, LocNode *loc)
        : StmtNode(line, col), myLoc(loc) {
    }
    void unparse(std::ostream& out, int indent) override = 0;
  private: 
    LocNode *myLoc;
};

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

class AssignExpNode : public ExpNode
{

};

class BinaryExpNode : public ExpNode
{

};

	/************************************
 	* BINARY EXPRESSION NODES
 	************************************/
 	class AndNode : public BinaryExpNode
	{

	};

	class DivideNode : public BinaryExpNode
	{

	};

	class EqualsNode : public BinaryExpNode
	{

	};

	class GreaterEqNode : public BinaryExpNode
	{

	};

	class GreaterNode : public BinaryExpNode
	{

	};

	class LessEqNode : public BinaryExpNode
	{

	};

	class LessNode : public BinaryExpNode
	{

	};

	class MinusNode : public BinaryExpNode
	{

	};

	class NotEqualsNode : public BinaryExpNode
	{

	};

	class OrNode : public BinaryExpNode
	{

	};

	class PlusNode : public BinaryExpNode
	{

	};

	class TimesNode : public BinaryExpNode
	{

	};

class CallExpNode : public ExpNode
{

};

class CharLitNode : public ExpNode
{

};

class FalseNode : public ExpNode
{

};

class IntLitNode : public ExpNode
{

};

class LValNode : public ExpNode
{

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
		IDNode(IDToken * token) 
		: ExpNode(token->line(), token->col()), myStrVal(token->value()){
			myStrVal = token->value();
		}
		void unparse(std::ostream& out, int indent);
	private:
		/** The name of the identifier **/
		std::string myStrVal;
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

class TrueNode : public ExpNode
{

};

class UnaryExpNode : public ExpNode
{

};

	/************************************
 	* UNARY NODES
 	************************************/
 	class NegNode : public UnaryExpNode
	{

	};

	class NotNode : public UnaryExpNode
	{

	};


/*****************************
 * TYPE NODES
 ****************************/
/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in 
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/

class BoolTypeNode : TypeNode{

};

class CharTypeNode : public TypeNode{

};

class IntTypeNode : public TypeNode
{
	public:
		IntTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
			: TypeNode(lineIn, colIn, isRefIn)
		{
		}
		void unparse(std::ostream &out, int indent);
};

class VoidTypeNode : public TypeNode{

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

class FnDeclNode : public DeclNode
{
  public:
    FnDeclNode(size_t line, size_t col, TypeNode *type, IDNode *id, std::list<DeclNode *> * decl_nodes, std::list<StmtNode *> * stmt_nodes)
        : DeclNode(type->line(), type->col()), myType(type), myId(id), formalDeclNodes(decl_nodes), stmtNodes(stmt_nodes)
    {
    }
    
    void unparse(std::ostream &out, int indent);

  private: 
    TypeNode *myType;
    IDNode *myId;
    std::list<DeclNode *> *formalDeclNodes;
    std::list<StmtNode *> *stmtNodes;
};

  class VarDeclNode : public DeclNode
  {
  public:
    VarDeclNode(size_t line, size_t col, TypeNode *type, IDNode *id)
        : DeclNode(type->line(), type->col()), myType(type), myId(id)
    {
    }
    void unparse(std::ostream &out, int indent);

  private:
    TypeNode *myType;
    IDNode *myId;
  };



} //End namespace holeyc

#endif
