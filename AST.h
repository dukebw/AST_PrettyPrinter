// To do: go through and replace all bare pointers with std::shared_ptr from <memory>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <random>

class BadPath{}; // For throwing file-existence errors
class BadSize{}; // For throwing range errors
class BadArgument{}; // for coming across the wrong operator

//------------------------------------------------------------------------------------

template <typename T> int signum(T val)
{
   return (val > T(0)) - (val < T(0));
}

//------------------------------------------------------------------------------------

// This random number generator will produce a DIFFERENT sequence of random numbers
// each time the program is run. If the SAME sequence of random numbers is desired
// for testing purposes use a std::default_random_engine member re and return dist(re);
class Rand_int {
public:
   Rand_int(int low, int high) :dist{low, high}, gen{rd()} {}
   int operator()() { return dist(gen); }
private:
   std::uniform_int_distribution<> dist;
   std::random_device rd;
   std::mt19937 gen;
};

//------------------------------------------------------------------------------------

enum class Type {
   VOID, BOOL, INT
};

//------------------------------------------------------------------------------------

enum class InfixOperator {
   LESS_EQUALS, EQUALS, PLUS, TIMES, ASSIGNMENT
};

//------------------------------------------------------------------------------------

enum class PostfixOperator {
   INCREMENT, DECREMENT
};

//------------------------------------------------------------------------------------

// E.g. returns "void" for Type::VOID
std::string typeToString(const Type t);

//------------------------------------------------------------------------------------

std::string infixOpToString(const InfixOperator infixOperator);

//------------------------------------------------------------------------------------

std::string postfixOpToString(const PostfixOperator postfixOperator);

//------------------------------------------------------------------------------------

std::string infixOpToStringJs(const InfixOperator infixOperator);

//------------------------------------------------------------------------------------

std::string infixOpToStringScm(const InfixOperator infixOperator);

//------------------------------------------------------------------------------------

std::string postfixOpToStringScm(const PostfixOperator postfixOperator);

//------------------------------------------------------------------------------------

// For declaring functions: contains the type and name.
struct Parameter {
   Type type;
   std::string name;
};

//------------------------------------------------------------------------------------

// Abstract superclass of all ASTNode types
class ASTNode;

struct TesterBoilerplate;
struct Boilerplate;
struct MethodDeclaration;
struct VarDeclStatement;
struct AssertStatement;
struct Block;
struct ReturnStatement;
struct AssignmentStatement;
struct IfStatement;
struct ForStatement;
struct Name;
struct BooleanLiteral;
struct NumberLiteral;
struct InfixExpression;
struct PostfixExpression;
struct VarDeclFragment;

//------------------------------------------------------------------------------------

class ASTVisitor {
public:
   virtual void visit(TesterBoilerplate* tester) = 0;
   virtual void visit(Boilerplate* boilerplate) = 0;
   virtual void visit(MethodDeclaration* methodDeclaration) = 0;
   virtual void visit(VarDeclStatement* varDeclStatement) = 0;
   virtual void visit(AssertStatement* assert) = 0;
   virtual void visit(Block* block) = 0;
   virtual void visit(ReturnStatement* returnStatement) = 0;
   virtual void visit(AssignmentStatement* assignmentStatement) = 0;
   virtual void visit(IfStatement* ifStatement) = 0;
   virtual void visit(ForStatement* ForStatement) = 0;
   virtual void visit(Name* name) = 0;
   virtual void visit(BooleanLiteral* booleanLiteral) = 0;
   virtual void visit(NumberLiteral* numberLiteral) = 0;
   virtual void visit(InfixExpression* infixExpression) = 0;
   virtual void visit(PostfixExpression* postfixExpression) = 0;
   virtual void visit(VarDeclFragment* varDeclFragment) = 0;
};

//------------------------------------------------------------------------------------

struct Printer : ASTVisitor {
   Printer(const std::string& indentType) :m_indentType{indentType} {}

   virtual void visit(TesterBoilerplate* tester) = 0;
   virtual void visit(Boilerplate* boilerplate) = 0;
   virtual void visit(MethodDeclaration* methodDeclaration) = 0;
   virtual void visit(VarDeclStatement* varDeclStatement) = 0;
   virtual void visit(AssertStatement* assert) = 0;
   virtual void visit(Block* block) = 0;
   virtual void visit(ReturnStatement* returnStatement) = 0;
   virtual void visit(AssignmentStatement* assignmentStatement) = 0;
   virtual void visit(IfStatement* ifStatement) = 0;
   virtual void visit(ForStatement* ForStatement) = 0;
   virtual void visit(Name* name) = 0;
   virtual void visit(BooleanLiteral* booleanLiteral) = 0;
   virtual void visit(NumberLiteral* numberLiteral) = 0;
   virtual void visit(InfixExpression* infixExpression) = 0;
   virtual void visit(PostfixExpression* postfixExpression) = 0;
   virtual void visit(VarDeclFragment* varDeclFragment) = 0;

   void setOutStream(std::ostream& os) { m_os = &os; }
   void printIndents() const;
   void printIntVector(const std::vector<int>& v) const;
   void incrementIndents() { ++m_indents; }
   void decrementIndents() { --m_indents; }
   void setIndents(int i) { m_indents = i; }
protected:
   std::ostream* m_os = &std::cout;
   std::string m_indentType;
private:
   int m_indents{0};
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

// For now just a quick-fix to get the boilerplate for A1Test.java etc. up.
struct TesterBoilerplate : ASTNode {
   TesterBoilerplate(const std::string& packageName, const std::string& className,
         const std::string& methodName, const std::string& name, 
         ASTNode* parent = nullptr)
      :ASTNode{parent}, m_packageName{packageName}, m_className{className}, 
      m_methodName{methodName}, m_name{name} {}

   ~TesterBoilerplate(); 

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::string& getName() const { return m_name; }
   const std::string& getPackageName() const { return m_packageName; }
   const std::string& getClassName() const { return m_className; }
   const std::string& getMethodName() const { return m_methodName; }
   const std::vector<AssertStatement*>& getAsserts() const { return m_asserts; }
   void addAssert(AssertStatement* assert) { m_asserts.push_back(assert); }
private:
   std::string m_packageName, m_className, m_methodName;
   std::string m_name;
   std::vector<AssertStatement*> m_asserts;
};

//------------------------------------------------------------------------------------

struct Declaration : ASTNode {
   virtual ~Declaration() {}
protected:
   Declaration(ASTNode* parent)
      :ASTNode{parent} {}
}; // Base class for all declarations (also virtual)

//------------------------------------------------------------------------------------

struct Statement : ASTNode {
   virtual ~Statement() {}
protected:
   Statement(ASTNode* parent)
      :ASTNode{parent} {}
}; // Base class for all statements (also virtual)

//------------------------------------------------------------------------------------

struct Expression : ASTNode {
   virtual ~Expression() {}
protected:
   Expression(ASTNode* parent)
      :ASTNode{parent} {}
};
 // Base class for all expressions (also virtual)

//------------------------------------------------------------------------------------

struct Boilerplate : Declaration {
   Boilerplate(const std::vector<std::string>& names, ASTNode* parent = nullptr)
      :Declaration{parent}, m_names{names} {}
   Boilerplate(const std::vector<std::string>& names, 
         const std::vector<Declaration*>& declarations, ASTNode* parent = nullptr)
      :Declaration{parent}, m_names{names}, m_decls{declarations} {}

   ~Boilerplate() { for (Declaration* d : m_decls) if (d) delete d; }

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::string& getName(unsigned i) const { return m_names.at(i); }
   std::vector<Declaration*>& getBodyDeclarations() { return m_decls; }

   void setName(unsigned i, const std::string& name) { m_names.at(i) = name; }
   void addDeclaration(Declaration* declaration) { m_decls.push_back(declaration); }
private:
   std::vector<std::string> m_names;
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

struct VarDeclStatement : Statement {
   VarDeclStatement(std::vector<VarDeclFragment*> varDecls, Type type, 
         ASTNode* parent = nullptr)
      :Statement{parent}, m_varDecls{varDecls}, m_type{type} {}

   ~VarDeclStatement();

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   VarDeclFragment* getVarDeclFragment(int i) 
   { 
      return m_varDecls.at(i); 
   }

   const std::vector<VarDeclFragment*>& getFragments() { return m_varDecls; }
   Type getType() const { return m_type; }
private:
   std::vector<VarDeclFragment*> m_varDecls;
   Type m_type;
};

//------------------------------------------------------------------------------------

struct AssertStatement : Statement {
   AssertStatement(const std::string& methodName, const std::vector<int>& parameters,
         const int result, ASTNode* parent = nullptr) 
   :Statement{parent}, m_methodName{methodName}, m_params{parameters}, m_res{result} {}

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::string& getMethodName() const { return m_methodName; }
   const std::vector<int>& getParameters() const { return m_params; }
   const int getResult() const { return m_res; }
private:
   std::string m_methodName;
   std::vector<int> m_params;
   int m_res;
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

struct AssignmentStatement : Statement {
   AssignmentStatement(Name* name, Expression* expression, ASTNode* parent = nullptr)
      :Statement{parent}, m_name{name}, m_expr{expression} {}

   ~AssignmentStatement();

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   Name* getName() { return m_name; }
   Expression* getExpression() { return m_expr; }
private:
   Name* m_name;
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

struct ForStatement : Statement {
   // Initializers should consist of either a single VarDeclExpression, or
   // a sequence of assignment expressions. Updaters should consist of a sequence of
   // statement-expressions. To do: add checks.
   ForStatement(std::vector<VarDeclFragment*> initializers, Expression* expression, 
        std::vector<Expression*> updaters, Statement* bodyStatement, 
        ASTNode* parent = nullptr)
      :Statement{parent}, m_forInits{initializers}, m_expr{expression}, 
      m_updaters{updaters}, m_body{bodyStatement} {}

   ~ForStatement();

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   const std::vector<VarDeclFragment*>& getInitializers() { return m_forInits; }
   Expression* getExpression() { return m_expr; }
   const std::vector<Expression*>& getUpdaters() { return m_updaters; }
   Statement* getBody() { return m_body; }
private:
   std::vector<VarDeclFragment*> m_forInits;
   Expression* m_expr;
   std::vector<Expression*> m_updaters;
   Statement* m_body;
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
   InfixExpression(Expression* leftHand, const InfixOperator infixOperator, 
         Expression* rightHand, ASTNode* parent = nullptr)
      :Expression{parent}, m_leftHand{leftHand}, m_op{infixOperator}, 
      m_rightHand{rightHand} {}

   ~InfixExpression();

   virtual void accept(ASTVisitor* visitor) { visitor->visit(this); }

   Expression* getLeftOperand() { return m_leftHand; }
   InfixOperator getOperator() const { return m_op; }
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

struct PostfixExpression : Expression {
   PostfixExpression(Expression* leftHand, const PostfixOperator postfixOperator,
         ASTNode* parent = nullptr)
      :Expression{parent}, m_leftHand{leftHand}, m_op{postfixOperator} {}

   ~PostfixExpression() { if (m_leftHand) delete m_leftHand; }

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   Expression* getLeftOperand() { return m_leftHand; }
   PostfixOperator getOperator() const { return m_op; }
private:
   Expression* m_leftHand;
   PostfixOperator m_op;
};

//------------------------------------------------------------------------------------

struct VarDeclFragment : Expression {
   VarDeclFragment(Name* name, Expression* expression, Type type, 
         ASTNode* parent = nullptr)
      :Expression{parent}, m_leftHand{name}, m_rightHand{expression}, m_type{type} {}

   void accept(ASTVisitor* visitor) { visitor->visit(this); }

   Name* getLeftOperand() { return m_leftHand; }
   Expression* getRightOperand() { return m_rightHand; }
   Type getType() const { return m_type; }
private:
   Name* m_leftHand;
   Expression* m_rightHand;
   Type m_type;
};

//------------------------------------------------------------------------------------

struct JavaPrinter : Printer {
   JavaPrinter() :Printer{"\t"} {}

   void visit(TesterBoilerplate* tester);
   void visit(Boilerplate* boilerplate);
   void visit(MethodDeclaration* methodDeclaration);
   void visit(VarDeclStatement* varDeclStatement);
   void visit(AssertStatement* assert);
   void visit(Block* block);
   void visit(ReturnStatement* returnStatement);
   void visit(AssignmentStatement* assignmentStatement);
   void visit(IfStatement* ifStatement);
   void visit(ForStatement* forStatement);
   void visit(Name* name) { *m_os << name->getName(); }
   void visit(BooleanLiteral* booleanLiteral);
   void visit(NumberLiteral* numberLiteral);
   void visit(InfixExpression* infixExpression);
   void visit(PostfixExpression* postfixExpression);
   void visit(VarDeclFragment* varDeclFragment);
};

//------------------------------------------------------------------------------------

struct JavaScriptPrinter : Printer {
   JavaScriptPrinter() :Printer{"\t"} {}

   void visit(TesterBoilerplate* tester);
   void visit(Boilerplate* boilerplate);
   void visit(MethodDeclaration* methodDeclaration);
   void visit(VarDeclStatement* varDeclStatement);
   void visit(AssertStatement* assert);
   void visit(Block* block);
   void visit(ReturnStatement* returnStatement);
   void visit(AssignmentStatement* assignmentStatement);
   void visit(IfStatement* ifStatement);
   void visit(ForStatement* forStatement);
   void visit(Name* name) { *m_os << name->getName(); }
   void visit(BooleanLiteral* booleanLiteral);
   void visit(NumberLiteral* numberLiteral);
   void visit(InfixExpression* infixExpression);
   void visit(PostfixExpression* postfixExpression);
   void visit(VarDeclFragment* varDeclFragment);
};

//------------------------------------------------------------------------------------

// Class to deal with missing brackets, which occur because the AST here is 
// constructed with lists of statements in a block, when really each 
// statement in the same block should probably be a child of its subsequent statement.
struct MissingBracket : Printer {
   MissingBracket() :Printer{"  "} {}

   void visit(TesterBoilerplate* tester) {}
   void visit(Boilerplate* boilerplate) {}
   void visit(MethodDeclaration* methodDeclaration) {}
   void visit(VarDeclStatement* varDeclStatement) { *m_os << ')'; }
   void visit(AssertStatement* assert) {}
   void visit(Block* block) {}
   void visit(ReturnStatement* returnStatement) {}
   void visit(AssignmentStatement* assignmentStatement) {}
   void visit(IfStatement* ifStatement);
   void visit(ForStatement* forStatement) {}
   void visit(Name* name) {}
   void visit(BooleanLiteral* booleanLiteral) {}
   void visit(NumberLiteral* numberLiteral) {}
   void visit(InfixExpression* infixExpression) {}
   void visit(PostfixExpression* postfixExpression) {}
   void visit(VarDeclFragment* varDeclFragment) {}
};

//------------------------------------------------------------------------------------

struct SchemePrinter : Printer {
   SchemePrinter() :Printer{"  "} {}

   void visit(TesterBoilerplate* tester);
   void visit(Boilerplate* boilerplate);
   void visit(MethodDeclaration* methodDeclaration);
   void visit(VarDeclStatement* varDeclStatement);
   void visit(AssertStatement* assert);
   void visit(Block* block);
   void visit(ReturnStatement* returnStatement);
   void visit(AssignmentStatement* assignmentStatement);
   void visit(IfStatement* ifStatement);
   void visit(ForStatement* forStatement);
   void visit(Name* name) { *m_os << name->getName(); }
   void visit(BooleanLiteral* booleanLiteral);
   void visit(NumberLiteral* numberLiteral);
   void visit(InfixExpression* infixExpression);
   void visit(PostfixExpression* postfixExpression);
   void visit(VarDeclFragment* varDeclFragment);
};

//------------------------------------------------------------------------------------

struct ResultFinder : ASTVisitor {
   ResultFinder(const std::vector<int>& inputs, 
         const std::vector<std::string> inputNames, ASTNode* parent = nullptr)
      :m_in{inputs}, m_inNames{inputNames} {}

   void visit(TesterBoilerplate* tester) {} // Shouldn't be used
   void visit(Boilerplate* boilerplate);
   void visit(MethodDeclaration* methodDeclaration);
   void visit(VarDeclStatement* varDeclStatement);
   void visit(AssertStatement* assert) {} // Shouldn't be used
   void visit(Block* block);
   void visit(ReturnStatement* returnStatement);
   void visit(AssignmentStatement* assignmentStatement);
   void visit(IfStatement* ifStatement);
   void visit(ForStatement* forStatement);
   void visit(Name* name);
   void visit(BooleanLiteral* booleanLiteral);
   void visit(NumberLiteral* numberLiteral);
   void visit(InfixExpression* infixExpression);
   void visit(PostfixExpression* postfixExpression);
   void visit(VarDeclFragment* varDeclFragment);

   const std::string& getResult() const { return m_res; }

   void addInput(const int i) { m_in.push_back(i); }
   // Add check for uniqueness
   void addInputName(const std::string& s) { m_inNames.push_back(s); }
private:
   std::string m_res;
   // m_in and m_inNames work as a simple Symbol Table.
   std::vector<int> m_in;
   std::vector<std::string> m_inNames;

   std::string m_compareName;
   int m_compareVal{0};
   bool m_compare{false};
   InfixOperator m_op{InfixOperator::LESS_EQUALS};
};

//------------------------------------------------------------------------------------

// To do: include error checking for successful directory creation.
void makeDirectories(const std::string& studentNumber, const std::string& language);

//------------------------------------------------------------------------------------

// Specifically prints the output of node->accept(printer) (i.e. printer->visit(node)) 
// to the file "fileName", which is created 
void writeToFile(const boost::filesystem::path& path, const std::string& fileName, 
      Printer* printer, ASTNode* node);

//------------------------------------------------------------------------------------

void readStudentNumbers(const std::string& fileName, std::vector<std::string>& v);

//------------------------------------------------------------------------------------

void createReturnBlocks(const std::vector<std::string>& returnNumbers, 
      std::vector<Block*>& returnBlocks);

//------------------------------------------------------------------------------------

void createInfixExpressions(const std::vector<std::string>& names, 
      const std::vector<int>& numbers, 
      std::vector<InfixExpression*>& infixExpressions);

//------------------------------------------------------------------------------------

void createInfixExpressions(const std::vector<std::string>& names, 
      const std::vector<std::string>& numbers, 
      std::vector<InfixExpression*>& infixExpressions);

//------------------------------------------------------------------------------------

// Even statements are then-statements; odd statements are else-statements
void createIfBlocks(const std::vector<InfixExpression*>& infixExpressions, 
      const std::vector<Block*>& statements, std::vector<Block*>& ifBlocks);

//------------------------------------------------------------------------------------

Boilerplate* createBoilerPlate(const std::string& packageName, 
      const std::string& className, Block* methodBlock, 
      const std::string& methodName, const std::vector<Parameter>& methodParameters, 
      const Type methodType);

//------------------------------------------------------------------------------------

void createIfTreeBlock(const std::vector<std::string>& returnValues,
      const std::vector<std::string>& orderedVariables, 
      const std::vector<std::string> testNumbers, Block* myBlock);

//------------------------------------------------------------------------------------

void randomizeTree(std::vector<std::string>& returnValues, 
      std::vector<std::string>& ifNames, std::vector<std::string>& ifNumbers);

//------------------------------------------------------------------------------------

// Fills vec with uniformly distributed integers from -range to range about 0,
// in string form ("0", "1", etc.)
void fillRandomVector(const unsigned numberToFill, const int range, 
      std::vector<std::string>& outputVec, const bool unique = false);

//------------------------------------------------------------------------------------

void printAssert_js(const boost::filesystem::path& assertPath);

//------------------------------------------------------------------------------------

void printAssert_scm(const boost::filesystem::path& assertPath);

//------------------------------------------------------------------------------------

void printA1A2(Printer* myPrinter, const std::string& studentNumber, 
      const std::string& language);

//------------------------------------------------------------------------------------

void createInitialization(std::vector<VarDeclFragment*>& a0a1anFragments,
      std::vector<VarDeclFragment*>& xyFragments);

//------------------------------------------------------------------------------------

void createForBlock(Block* myBlock);

//------------------------------------------------------------------------------------

void createRecurrenceBlock(Block* myBlock);

//------------------------------------------------------------------------------------

void printA3(Printer* myPrinter, const std::string& studentNumber,
      const std::string& language);

//------------------------------------------------------------------------------------

void printA1A2Tests(Printer* myPrinter, Boilerplate* myProgram, 
      const std::string& className, const std::string& methodName, 
      const std::string& name, boost::filesystem::path studentPath);

//------------------------------------------------------------------------------------

void printA3Tests(Printer* myPrinter, Boilerplate* myProgram, 
      const std::string& className, const std::string& methodName,
      const std::string& name, boost::filesystem::path studentPath);
