// To do: go through and replace all bare pointers with std::shared_ptr from <memory>
#include <string>
#include <vector>
#include <iostream>
#include <exception>

class BadSize{};  // For throwing range errors from getter functions.

//------------------------------------------------------------------------------------

enum class Type {
   VOID, BOOL, INT
};

//------------------------------------------------------------------------------------

enum class InfixOperator {
   LESS_EQUALS, EQUALS, TIMES
};

//------------------------------------------------------------------------------------

// E.g. returns "void" for Type::VOID
std::string typeToString(const Type t);

//------------------------------------------------------------------------------------

std::string infixOpToString(const InfixOperator infixOperator);

//------------------------------------------------------------------------------------

// For declaring functions: contains the type and name.
struct Parameter {
   Type type;
   std::string name;
};

//------------------------------------------------------------------------------------

// Abstract superclass of all ASTNode types
class ASTNode;

struct PackageDeclaration;
struct ClassDeclaration;
struct MethodDeclaration;
struct Block;
struct ReturnStatement;
struct IfStatement;
struct Name;
struct BooleanLiteral;
struct NumberLiteral;
struct InfixExpression;

//------------------------------------------------------------------------------------

class ASTVisitor {
public:
   virtual void visit(PackageDeclaration* packageDeclaration) = 0;
   virtual void visit(ClassDeclaration* classDeclaration) = 0;
   virtual void visit(MethodDeclaration* methodDeclaration) = 0;
   virtual void visit(Block* block) = 0;
   virtual void visit(ReturnStatement* returnStatement) = 0;
   virtual void visit(IfStatement* ifStatement) = 0;
   virtual void visit(Name* name) = 0;
   virtual void visit(BooleanLiteral* booleanLiteral) = 0;
   virtual void visit(NumberLiteral* numberLiteral) = 0;
   virtual void visit(InfixExpression* infixExpression) = 0;
};

//------------------------------------------------------------------------------------

class ASTNode {
public:
   virtual void accept(ASTVisitor* visitor) = 0;
   // add delete() function? ASTNode* m_parent and ASTNode* m_child members?
   // Virtual destructor so that proper subclass destructor is called if that
   // subclass is deleted through a pointer to its base class.
   virtual ~ASTNode() {}
protected:
   ASTNode(ASTNode* parent)
      :m_parent{parent} {}
private:
   // m_parent currently unused. Consider removing, or implementing features that
   // use traversing up a tree, if needed?
   ASTNode* m_parent;
};

//------------------------------------------------------------------------------------

struct PackageDeclaration : ASTNode {
   PackageDeclaration(const std::string& name, ClassDeclaration* classDeclaration,
         ASTNode* parent = nullptr)
      :ASTNode{parent}, m_name{name}, m_classDecl{classDeclaration} {}

   ~PackageDeclaration();

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::string& getName() { return m_name; }
   ClassDeclaration* getClassDeclaration() { return m_classDecl; }
private:
   std::string m_name;
   ClassDeclaration* m_classDecl;
};

//------------------------------------------------------------------------------------

struct Declaration : ASTNode {
protected:
   Declaration(ASTNode* parent)
      :ASTNode{parent} {}
}; // Base class for all declarations (also virtual)

//------------------------------------------------------------------------------------

struct Statement : ASTNode {
protected:
   Statement(ASTNode* parent)
      :ASTNode{parent} {}
}; // Base class for all statements (also virtual)

//------------------------------------------------------------------------------------

struct Expression : ASTNode {
protected:
   Expression(ASTNode* parent)
      :ASTNode{parent} {}
};
 // Base class for all expressions (also virtual)

//------------------------------------------------------------------------------------

struct ClassDeclaration : Declaration {
   ClassDeclaration(const std::string& name, ASTNode* parent = nullptr)
      :Declaration{parent}, m_name{name} {}
   ClassDeclaration(const std::string& name, 
         const std::vector<Declaration*>& declarations, ASTNode* parent = nullptr)
      :Declaration{parent}, m_name{name}, m_decls{declarations} {}

   ~ClassDeclaration() { for (Declaration* d : m_decls) if (d) delete d; }

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::string& getName() const { return m_name; }
   const std::vector<Declaration*>& getBodyDeclarations() { return m_decls; }

   void setName(const std::string& name) { m_name = name; }
   void addDeclaration(Declaration* declaration) { m_decls.push_back(declaration); }
private:
   std::string m_name;
   // could be MethodDeclarations, FieldDeclarations etc.
   std::vector<Declaration*> m_decls; 
};

//------------------------------------------------------------------------------------

// For now all methods are declared with the "public" modifier. To correct this,
// MethodDeclaration (and ClassDeclaration) would need a modifier member type.
struct MethodDeclaration : Declaration {
   MethodDeclaration(Block* block, const std::string& name, 
         const std::vector<Parameter>& parameters, Type returnType = Type::VOID, 
         ASTNode* parent = nullptr)
      :Declaration{parent}, m_body{block}, m_name{name}, m_params{parameters}, 
      m_returnType{returnType} {}
   MethodDeclaration(Block* block, const std::string& name, 
         Type returnType = Type::VOID, ASTNode* parent = 0)
      :Declaration{parent}, m_body{block}, m_name{name}, m_returnType{returnType} {}

   ~MethodDeclaration();

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   Block* getBody() { return m_body; }
   const std::string& getName() const { return m_name; }
   const std::vector<Parameter>& getParamList() const { return m_params; }
   Type getReturnType() const { return m_returnType; }

   void setReturnType(Type type) { m_returnType = type; }
   void addParameter(const Parameter& parameter) { m_params.push_back(parameter); }
private:
   Block* m_body;
   std::string m_name;
   std::vector<Parameter> m_params;
   Type m_returnType;
};

//------------------------------------------------------------------------------------

struct Block : Statement {
   Block(ASTNode* parent = nullptr)
      :Statement{parent} {}
   Block(const std::vector<Statement*>& statements, ASTNode* parent = nullptr)
      :Statement{parent}, m_stmts{statements} {}

   // frees up memory on the heap
   ~Block() { for (Statement* s : m_stmts) if (s) delete s; }

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   // reference to the vector m_stmts? Should it return a pointer?
   const std::vector<Statement*>& getStatements() { return m_stmts; } 

   void addStatement(Statement* statement) { m_stmts.push_back(statement); }
private:
   std::vector<Statement*> m_stmts;
};

//------------------------------------------------------------------------------------

struct ReturnStatement : Statement {
   ReturnStatement(Expression* expression, ASTNode* parent = nullptr)
      :Statement{parent}, m_expr{expression} {}

   ~ReturnStatement() { if (m_expr) delete m_expr; }

   void accept(ASTVisitor* visitor) { visitor->visit(this); } 

   Expression* getExpression() { return m_expr; }

   // Want to delete existing m_expr (if it exists) (not yet implemented)
   void setExpression(Expression* expression) { m_expr = expression; }
private:
   Expression* m_expr;
};

//------------------------------------------------------------------------------------

struct IfStatement : Statement {
   IfStatement(Expression* expression, Statement* thenStatement,
         Statement* elseStatement = nullptr, ASTNode* parent = nullptr)
      :Statement{parent}, m_expr{expression}, m_thenStmt{thenStatement}, 
      m_elseStmt{elseStatement} {}

   ~IfStatement();

   void accept(ASTVisitor* visitor) { visitor->visit(this); } 

   // make getExpression(), getThenStatement() etc. all const?
   // returns Expression of if-statement
   Expression* getExpression()   { return m_expr; }
   Statement* getThenStatement() { return m_thenStmt; }
   // returns Else part of Statement
   Statement* getElseStatement() { return m_elseStmt; }

   void setExpression(Expression* expression);
   // throw Exceptions if Node has a parent (not yet implemented)
   void setThenStatement(Statement* statement);
   void setElseStatement(Statement* statement);
private:   
   Expression* m_expr;
   Statement* m_thenStmt;
   Statement* m_elseStmt;
};

//------------------------------------------------------------------------------------

struct Name : Expression {
   Name(const std::string& name, ASTNode* parent = nullptr)
      :Expression{parent}, m_name{name} {}

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::string& getName() const { return m_name; }
   void setName(const std::string& name) { m_name = name; }
private:
   std::string m_name;
};

//------------------------------------------------------------------------------------

struct BooleanLiteral : Expression {
   BooleanLiteral(const bool value, ASTNode* parent = nullptr)
      :Expression{parent}, m_boolVal{value} {}

   void accept(ASTVisitor* visitor) { visitor->visit(this); } 

   bool booleanValue() { return m_boolVal; }

   void setBooleanValue(const bool value) { m_boolVal = value; }
private:
   bool m_boolVal;
};

//------------------------------------------------------------------------------------

struct NumberLiteral : Expression {
   // should throw an exception if token isn't a string of digits (no floats for now)
   NumberLiteral(const std::string& token, ASTNode* parent = nullptr)
      :Expression{parent}, m_token{token} {}

   void accept(ASTVisitor* visitor) { visitor->visit(this); } 

   std::string getToken() { return m_token; }

   void setToken(const std::string& token) { m_token = token; }
private:
   std::string m_token; 
};

//------------------------------------------------------------------------------------

struct InfixExpression : Expression {
   InfixExpression(Expression* leftHand, InfixOperator infixOperator, 
         Expression* rightHand, ASTNode* parent = nullptr)
      :Expression{parent}, m_leftHand{leftHand}, m_op{infixOperator}, 
      m_rightHand{rightHand} {}

   ~InfixExpression();

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   Expression* getLeftOperand() { return m_leftHand; }
   InfixOperator getOperator() { return m_op; }
   Expression* getRightOperand() { return m_rightHand; }

   void setLeftOperand(Expression* expression);
   void setOperator(const InfixOperator infixOperator) { m_op = infixOperator; }
   void setRightOperand(Expression* expression);
private:
   Expression* m_leftHand;
   InfixOperator m_op;
   Expression* m_rightHand;
};

//------------------------------------------------------------------------------------

struct JavaPrinter : ASTVisitor {
   void visit(PackageDeclaration* packageDeclaration);
   void visit(ClassDeclaration* classDeclaration);
   void visit(MethodDeclaration* methodDeclaration);
   void visit(Block* block);
   void visit(ReturnStatement* returnStatement);
   void visit(IfStatement* ifStatement);
   void visit(Name* name) { m_os << name->getName(); }
   void visit(BooleanLiteral* booleanLiteral);
   void visit(NumberLiteral* numberLiteral);
   void visit(InfixExpression* infixExpression);

   void printIndents() const;
   void incrementIndents() { ++m_indents; }
   void decrementIndents() { --m_indents; }
private:
   int m_indents{0};
   std::ostream& m_os = std::cout;
};
