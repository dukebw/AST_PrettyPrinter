#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <exception>

// Abstract superclass of all ASTNode types
class ASTVisitor;

class ASTNode {
public:
   virtual void accept(ASTVisitor* visitor) = 0;
   // add delete() function? ASTNode* m_parent and ASTNode* m_child members?
   // virtual destructor so that proper subclass destructor is called if that
   // subclass is deleted through a pointer to its base class
   virtual ~ASTNode() {}
protected:
   ASTNode(ASTNode* parent)
      :m_parent{parent} {}
private:
   std::unique_ptr<ASTNode> m_parent;
};

//------------------------------------------------------------------------------------

struct Statement : ASTNode {}; // Base class for all statements (also virtual)

//------------------------------------------------------------------------------------

struct Expression : ASTNode {}; // Base class for all expressions (also virtual)

//------------------------------------------------------------------------------------

struct Block : Statement {
   Block(std::vector<Statement*> statements, ASTNode* parent = nullptr)
      :m_stmts{statements}, ASTNode{parent}

   void accept(ASTVisitor* visitor)
   {
      visitor->visit(this);
   }

   std::vector<std::unique_ptr<Statement>> getStatements()
   {
      return m_stmts;
   } 
private:
   std::vector<std::unique_ptr<Statement>> m_stmts;
};
//------------------------------------------------------------------------------------

struct ReturnStatement : Statement {
   ReturnStatement(Expression* expression, ASTNode* parent = nullptr)
      :m_expr{expression}, ASTNode{parent} {}

   void accept(ASTVisitor* visitor)
   {
      visitor->visit(this);
   } 

   std::unique_ptr<Expression> getExpression()
   {
      return m_expr;
   }
   void setExpression(Expression* expression)
   {
      m_expr = expression;
   }
private:
   std::unique_ptr<Expression> m_expr;
};

//------------------------------------------------------------------------------------

struct IfStatement : Statement {
   IfStatement(Expression* expression, Statement* thenStatement,
         Statement* elseStatement = nullptr, ASTNode* parent = nullptr)
      :m_expr{expression}, m_thenStmt{thenStatement}, m_elseStmt{elseStatement},
      ASTNode{parent}

   void accept(ASTVisitor* visitor)
   {
      visitor->visit(this);
   } 

   // make getExpression(), getThenStatement() etc. all const?
   std::unique_ptr<Expression> getExpression()   // returns Expression of if-statement
   {
      return m_expr;
   }
   std::unique_ptr<Statement> getThenStatement()
   {
      return m_thenStmt;
   }
   std::unique_ptr<Statement> getElseStatement() // returns Else part of Statement
   {
      return m_elseStmt;
   }

   void setExpression(Expression* expression)
   {
      m_expr = expression;
   }
   // throw Exceptions if Node has a parent (not yet implemented)
   void setThenStatement(Statement* statement)
   {
      m_thenStmt = statement;
   }
   void setElseStatement(Statement* statement)
   {
      m_elseStmt = statement;
   } 
private:   
   std::unique_ptr<Expression> m_expr;
   std::unique_ptr<Statement> m_thenStmt;
   std::unique_ptr<Statement> m_elseStmt;
};

//------------------------------------------------------------------------------------

struct BooleanLiteral : Expression {
   BooleanLiteral(const bool value, ASTNode* parent = nullptr)
      :m_boolVal{value}, ASTNode{parent} {}

   void accept(ASTVisitor* visitor)
   {
      visitor->visit(this);
   } 

   bool booleanValue()
   {
      return m_boolVal;
   }

   void setBooleanValue(const bool value)
   {
      m_boolVal = value;
   }
private:
   bool m_boolVal;
};

//------------------------------------------------------------------------------------

struct NumberLiteral : Expression {
   // should throw an exception if token isn't a string of digits (no floats for now)
   NumberLiteral(const std::string& token, ASTNode* parent = nullptr)
      :m_token{token}, ASTNode{parent} {}

   void accept(ASTVisitor* visitor)
   {
      visitor->visit(this);
   } 

   std::string getToken()
   {
      return m_token;
   }

   void setToken(const std::string& token)
   {
      m_token = token;
   }
private:
   std::string m_token; 
};

//------------------------------------------------------------------------------------

class ASTVisitor {
   virtual visit(ASTNode* node) = 0;
};

//------------------------------------------------------------------------------------

struct JavaPrinter : ASTVisitor {
   void visit(const ReturnStatement& returnStatement);
   void visit(const IfStatement& ifStatement);
   void visit(const BooleanLiteral& booleanLiteral);
   void visit(const NumberLiteral& numberLiteral);

   void printIndents() const;
   void incrementIndents() { ++m_indents; }
   void decrementIndents() { --m_indents; }
private:
   int m_indents{0};
};
