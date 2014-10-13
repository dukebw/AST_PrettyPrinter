#include "AST.h"

//------------------------------------------------------------------------------------

std::string typeToString(const Type t)
{
   switch (t) {
      case Type::VOID:
         return "void";
      case Type::BOOL:
         return "boolean";
      case Type::INT:
         return "int";
      default:
         return "_ERROR_"; // shouldn't ever print
   }
}

//------------------------------------------------------------------------------------

std::string infixOpToString(const InfixOperator infixOperator)
{
   switch (infixOperator) {
      case InfixOperator::LESS_EQUALS:
         return "<=";
      case InfixOperator::EQUALS:
         return "==";
      case InfixOperator::PLUS:
         return "+";
      case InfixOperator::TIMES:
         return "*";
      case InfixOperator::ASSIGNMENT:
         return "=";
      default:
         return "_ERROR_";
   }
}

//------------------------------------------------------------------------------------

std::string postfixOpToString(const PostfixOperator postfixOperator)
{
   switch (postfixOperator) {
      case PostfixOperator::INCREMENT:
         return "++";
      case PostfixOperator::DECREMENT:
         return "--";
      default: 
         return "_ERROR_";
   }
}

//------------------------------------------------------------------------------------

TesterBoilerplate::~TesterBoilerplate()
{ 
   for (AssertStatement* as : m_asserts) if (as) delete as; 
}

//------------------------------------------------------------------------------------

MethodDeclaration::~MethodDeclaration()
{
   if (m_body) delete m_body;
}

//------------------------------------------------------------------------------------

VarDeclStatement::~VarDeclStatement() 
{ 
   for (VarDeclFragment* vd : m_varDecls) delete vd; 
}

//------------------------------------------------------------------------------------

AssignmentStatement::~AssignmentStatement()
{
   if (m_name) delete m_name;
   if (m_expr) delete m_expr;
}

//------------------------------------------------------------------------------------

IfStatement::~IfStatement()
{
   if (m_expr) delete m_expr;
   if (m_thenStmt) delete m_thenStmt;
   if (m_elseStmt) delete m_elseStmt;
}

//------------------------------------------------------------------------------------

void IfStatement::setExpression(Expression* expression)
{
   if (m_expr) delete m_expr;
   m_expr = expression;
}

//------------------------------------------------------------------------------------

void IfStatement::setThenStatement(Statement* statement)
{
   if (m_thenStmt) delete m_thenStmt;
   m_thenStmt = statement;
}

//------------------------------------------------------------------------------------

void IfStatement::setElseStatement(Statement* statement)
{
   if (m_elseStmt) delete m_elseStmt;
   m_elseStmt = statement;
}

//------------------------------------------------------------------------------------

ForStatement::~ForStatement()
{
   if (!m_forInits.empty()) for (VarDeclFragment* vdf : m_forInits) delete vdf;
   if (m_expr) delete m_expr;
   if (m_body) delete m_body;
   if (!m_updaters.empty()) for (Expression* e : m_updaters) delete e;
}
//------------------------------------------------------------------------------------

InfixExpression::~InfixExpression()
{
   if (m_leftHand) delete m_leftHand;
   if (m_rightHand) delete m_rightHand;
}

//------------------------------------------------------------------------------------

void InfixExpression::setLeftOperand(Expression* expression)
{
   if (m_leftHand) delete m_leftHand;
   m_leftHand = expression;
}

//------------------------------------------------------------------------------------

void InfixExpression::setRightOperand(Expression* expression)
{
   if (m_rightHand) delete m_rightHand;
   m_rightHand = expression;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(TesterBoilerplate* tester)
{  // Temporary fix
   *m_os << "import static org.junit.Assert.*;" << std::endl;
   *m_os << "import org.junit.BeforeClass;" << std::endl;
   *m_os << "import org.junit.Test;" << std::endl;
   *m_os << "import " << tester->getPackageName() << '.' << tester->getClassName()
      << ';' << std::endl;
   *m_os << "public class " << tester->getClassName() << " {" << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "private static " << tester->getClassName() << " tester;" << std::endl;
   printIndents();
   *m_os << "@BeforeClass" << std::endl;
   printIndents();
   *m_os << "public static void setUp() {" << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "tester = new " << tester->getClassName() << "();" << std::endl;
   decrementIndents();
   printIndents();
   *m_os << '}' << std::endl;
   printIndents();
   *m_os << "@Test" << std::endl;
   printIndents();
   *m_os << "public void " << tester->getMethodName() << "Test() {" << std::endl;
   incrementIndents();
   for (AssertStatement* as : tester->getAsserts()) {
      printIndents();
      as->accept(this);
   }
   decrementIndents();
   printIndents();
   *m_os << '}' << std::endl;
   decrementIndents();
   printIndents();
   *m_os << '}' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(Boilerplate* boilerplate)
{
   printIndents();
   *m_os << "package " << boilerplate->getName(0) << ';' << std::endl;
   printIndents();
   *m_os << "public class " << boilerplate->getName(1) << " {" << std::endl;
   incrementIndents();
   for (Declaration* d : boilerplate->getBodyDeclarations())
      d->accept(this);   

   decrementIndents();
   *m_os << '}' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(MethodDeclaration* methodDeclaration)
{
   printIndents();
   *m_os << "public " << typeToString(methodDeclaration->getReturnType()) << ' '
      << methodDeclaration->getName() << '(';
   
   // If statement formats argument list so it's (int x, int y, char z, ...),
   // or "()" if there are no arguments
   if (methodDeclaration->getParamList().size() > 0) {
      using size_type = std::vector<Parameter>::size_type;
      for (size_type i = 0; i<methodDeclaration->getParamList().size()-1; ++i) {
            const Parameter& p = methodDeclaration->getParamList().at(i);
            *m_os << typeToString(p.type) << ' ' << p.name << ", ";
      }

      const Parameter& p = methodDeclaration->getParamList().back();
      *m_os << typeToString(p.type) << ' ' << p.name << ") ";
   }
   else *m_os << ") ";

   methodDeclaration->getBody()->accept(this);
   *m_os << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(VarDeclStatement* varDeclStatement)
{
   *m_os << typeToString(varDeclStatement->getType()) << ' ';
   for (unsigned i=0; i<varDeclStatement->getFragments().size(); ++i) {
      varDeclStatement->getFragments().at(i)->accept(this);
      if (i != varDeclStatement->getFragments().size() - 1)
         *m_os << ", ";
   }
   *m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(AssertStatement* assert)
{
   *m_os << "assertEquals(\"" << assert->getMethodName() << '(';
   printIntVector(assert->getParameters());
   *m_os << ") must be " << assert->getResult() << "\", " << assert->getResult() << 
      ", tester." << assert->getMethodName() << '(';
   printIntVector(assert->getParameters());
   *m_os << "));" << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(Block* block)
{
   *m_os << '{' << std::endl;

   incrementIndents();
   // Print all the statements in the block (if any).
   // As of now all Indent printing is done by visiting Block.
   // If, for example, a thenStatement* or elseStatement* belonging to an 
   // ifStatement happen to not be blocks, then those statements are printed on
   // the same line as the "if (expression)" or "else", respectively.
   for (Statement* statement : block->getStatements()) {
      printIndents();
      statement->accept(this);
   }
   decrementIndents();

   printIndents();
   *m_os << '}';
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(ReturnStatement* returnStatement)
{
   *m_os << "return ";
   returnStatement->getExpression()->accept(this);
   *m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(AssignmentStatement* assignmentStatement)
{ 
   assignmentStatement->getName()->accept(this);
   *m_os << " = ";
   assignmentStatement->getExpression()->accept(this);
   *m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(IfStatement* ifStatement)
{
   *m_os << "if (";
   ifStatement->getExpression()->accept(this); // print expression
   *m_os << ") ";
   ifStatement->getThenStatement()->accept(this);
   
   if (ifStatement->getElseStatement()) {
      *m_os << " else ";
      ifStatement->getElseStatement()->accept(this);
      *m_os << std::endl;
   }
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(ForStatement* forStatement)
{
   *m_os << "for (";
   if (!forStatement->getInitializers().empty())
      *m_os << typeToString(forStatement->getInitializers().at(0)->getType()) << ' ';
   for (Expression* e : forStatement->getInitializers()) e->accept(this);
   *m_os << "; ";
   forStatement->getExpression()->accept(this);
   *m_os << "; ";
   for (Expression* e : forStatement->getUpdaters()) e->accept(this);
   *m_os << ") ";
   forStatement->getBody()->accept(this);
   *m_os << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(BooleanLiteral* booleanLiteral)
{
   if (booleanLiteral->booleanValue() == true)
      *m_os << "true";
   else *m_os << "false";
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(NumberLiteral* numberLiteral)
{
   std::string token{numberLiteral->getToken()};
   *m_os << token;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(InfixExpression* infixExpression)
{
   infixExpression->getLeftOperand()->accept(this);
   *m_os << ' ' << infixOpToString(infixExpression->getOperator()) << ' ';
   infixExpression->getRightOperand()->accept(this);
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(PostfixExpression* postfixExpression)
{
   postfixExpression->getLeftOperand()->accept(this);
   *m_os << postfixOpToString(postfixExpression->getOperator());
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(VarDeclFragment* varDeclFragment)
{
   varDeclFragment->getLeftOperand()->accept(this);
   *m_os << ' ' << infixOpToString(varDeclFragment->getOperator()) << ' ';
   varDeclFragment->getRightOperand()->accept(this);
}

//------------------------------------------------------------------------------------

void JavaPrinter::printIndents() const
{
   for (int i=0; i<m_indents; ++i)
      *m_os << '\t';
}

//------------------------------------------------------------------------------------

void JavaPrinter::printIntVector(const std::vector<int>& v) const
{
   if (v.empty()) return;
   if (v.size() > 1)
      for (int i=0; i < static_cast<int>(v.size())-1; ++i)
         *m_os << v.at(i) << ',';
   *m_os << v.back();
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(Boilerplate* boilerplate)
{
   for (Declaration* d : boilerplate->getBodyDeclarations())
      d->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(MethodDeclaration* methodDeclaration)
{
   methodDeclaration->getBody()->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(VarDeclStatement* varDeclStatement)
{
   for (VarDeclFragment* vdf : varDeclStatement->getFragments())
      vdf->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(Block* block)
{
   for (Statement* s : block->getStatements())
      s->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(ReturnStatement* returnStatement)
{
   returnStatement->getExpression()->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(AssignmentStatement* assignmentStatement)
{
   assignmentStatement->getName()->accept(this);
   // Check that we're assigning to a variable already in the symbol table:
   auto result = std::find(m_inNames.begin(), m_inNames.end(), m_compareName);
   if (result == m_inNames.end()) throw BadArgument{};

   // Evaluate the right-hand expression:
   assignmentStatement->getExpression()->accept(this);
   for (unsigned i=0; i<m_inNames.size(); ++i)
      if (m_inNames.at(i) == m_compareName)
         m_in.at(i) = m_compareVal;
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(IfStatement* ifStatement)
{
   ifStatement->getExpression()->accept(this);
   if (m_compare)
      ifStatement->getThenStatement()->accept(this);
   else
      ifStatement->getElseStatement()->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(ForStatement* forStatement)
{
   for (Expression* e : forStatement->getInitializers())
      e->accept(this);
   forStatement->getExpression()->accept(this);
   while (m_compare) {
      forStatement->getBody()->accept(this);
      for (Expression* e : forStatement->getUpdaters())
         e->accept(this);
      forStatement->getExpression()->accept(this);
   }
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(Name* name)
{
   m_compareName = name->getName();
   auto result = std::find(m_inNames.begin(), m_inNames.end(), m_compareName); 
   if (result == m_inNames.end()) throw BadArgument{};
   for (unsigned i=0; i<m_inNames.size(); ++i)
      if (m_inNames.at(i) == m_compareName)
         m_compareVal = m_in.at(i);
}
   
//------------------------------------------------------------------------------------

void ResultFinder::visit(BooleanLiteral* booleanLiteral)
{
   m_compare = true;
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(NumberLiteral* numberLiteral)
{
   m_compareVal = std::stoi(numberLiteral->getToken());
   m_res = numberLiteral->getToken();
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(InfixExpression* infixExpression)
{
   // Sets m_compareVal to the result of evaluating the InfixExpression (if it's
   // an integer expression), or m_compare to the result if it's a boolean expression
   infixExpression->getLeftOperand()->accept(this);
   m_op = infixExpression->getOperator();
   int temp{m_compareVal};
   infixExpression->getRightOperand()->accept(this);
   switch (m_op) {
      case InfixOperator::LESS_EQUALS:
         if (temp <= m_compareVal) m_compare = true;
         else m_compare = false;
         break;
      case InfixOperator::EQUALS:
         if (temp == m_compareVal) m_compare = true;
         else m_compare = false;
         break;
      case InfixOperator::PLUS:
         m_compareVal += temp;
         break;
      case InfixOperator::TIMES:
         m_compareVal *= temp;
         break;
      default:
         throw BadArgument{};
   }
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(PostfixExpression* postfixExpression)
{
   // A bit of a hack: only works with variable++ or variable--
   postfixExpression->getLeftOperand()->accept(this);
   for (unsigned i=0; i<m_inNames.size(); ++i)
      if (m_inNames.at(i) == m_compareName)
         switch(postfixExpression->getOperator()) {
            case PostfixOperator::INCREMENT:
               m_in.at(i) = ++m_compareVal;
               break;
            case PostfixOperator::DECREMENT:
               m_in.at(i) = --m_compareVal;
               break;
            default:
               throw BadArgument{};
               break;
         }
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(VarDeclFragment* varDeclFragment)
{
   varDeclFragment->getLeftOperand()->accept(this);
   auto result = std::find(m_inNames.begin(), m_inNames.end(), m_compareName);
   if (result != m_inNames.end()) throw BadArgument{};
   m_inNames.push_back(m_compareName);
   varDeclFragment->getRightOperand()->accept(this);
   m_in.push_back(m_compareVal);
}

//------------------------------------------------------------------------------------

void makeDirectories(const std::string& studentNumber)
{
   namespace bfs = boost::filesystem;
   // create_directory doesn't throw an error if directory creation fails due to
   // that directory already existing.
   bfs::create_directory(studentNumber);
   bfs::path studentNumberPath{studentNumber};
   bfs::create_directory(studentNumberPath / "se2s03");
}

//------------------------------------------------------------------------------------

void writeToFile(const boost::filesystem::path& path, const std::string& fileName,
      JavaPrinter* printer, ASTNode* node)
{
   // Writes to a file in path named fileName
   namespace bfs = boost::filesystem;
   if (!bfs::exists(path)) throw BadPath{};

   bfs::path filePath{bfs::path(path / fileName)};
   if (!bfs::exists(filePath)) {
      std::ofstream outFileStream{filePath.c_str()};
      printer->setOutStream(outFileStream);
      node->accept(printer);
      printer->setOutStream(std::cout);
   }
}

//------------------------------------------------------------------------------------

void readStudentNumbers(const std::string& fileName, std::vector<std::string>& v)
{
   std::ifstream ifs{fileName};
   if (!ifs) throw BadPath{};

   for (std::string s; ifs >> s;) v.push_back(s);
}

//------------------------------------------------------------------------------------

void createReturnBlocks(const std::vector<std::string>& returnValueStrings, 
      std::vector<Block*>& returnBlocks)
{
   std::vector<NumberLiteral*> returnNumbers;
   std::vector<ReturnStatement*> returnStatements;
   // Note that created NumberLiterals on heap shouldn't be deleted, since they're
   // still pointed to by the Block*s' child pointers.
   for (const std::string& s : returnValueStrings) {
      returnNumbers.push_back(new NumberLiteral{s});
      returnStatements.push_back(new ReturnStatement{returnNumbers.back()});
      returnBlocks.push_back(new Block);
      returnBlocks.back()->addStatement(returnStatements.back());
   }
}

//------------------------------------------------------------------------------------

void createInfixExpressions(const std::vector<std::string>& names, 
      const std::vector<std::string>& numbers, 
      std::vector<InfixExpression*>& infixExpressions)
{
   if (names.size() != numbers.size()) throw BadSize{};
   std::vector<Name*> infixVariables;
   std::vector<NumberLiteral*> infixNumbers;
   for (unsigned i=0; i<names.size(); ++i) {
      infixVariables.push_back(new Name{names.at(i)});
      infixNumbers.push_back(new NumberLiteral{numbers.at(i)});
      infixExpressions.push_back(new InfixExpression{infixVariables.back(),
            InfixOperator::LESS_EQUALS, infixNumbers.back()});
   }
}

//------------------------------------------------------------------------------------

// Even statements are then-statements; odd statements are else-statements
void createIfBlocks(const std::vector<InfixExpression*>& infixExpressions, 
      const std::vector<Block*>& statements, std::vector<Block*>& ifBlocks)
{
   if (statements.size() % 2 != 0) throw BadSize{}; // Must have even # of statements
   if (infixExpressions.size() != statements.size() / 2) throw BadSize{};
   for (unsigned i=0; i<infixExpressions.size(); ++i) {
      IfStatement* tempIf{new IfStatement{infixExpressions.at(i),
         statements.at(2*i), statements.at(2*i+1)}};
      ifBlocks.push_back(new Block);
      ifBlocks.back()->addStatement(tempIf);
   }
}

//------------------------------------------------------------------------------------

Boilerplate* createBoilerPlate(const std::string& packageName, 
      const std::string& className, Block* methodBlock, 
      const std::string& methodName, const std::vector<Parameter>& methodParameters, 
      const Type methodType)
{
   MethodDeclaration* myMethod{new MethodDeclaration{methodBlock, methodName, 
      methodParameters, methodType}};
   Boilerplate* myProgram{new Boilerplate{std::vector<std::string>{packageName, 
      className}}};
   myProgram->addDeclaration(myMethod);
   return myProgram;
}

//------------------------------------------------------------------------------------

void createIfTreeBlock(const std::vector<std::string>& returnValues,
      const std::vector<std::string>& ifNames, 
      const std::vector<std::string> ifNumbers, Block* myBlock)
{
   std::vector<Block*> returnBlocks;
   createReturnBlocks(returnValues, returnBlocks);
   
   std::vector<InfixExpression*> infixExpressions;
   createInfixExpressions(ifNames, ifNumbers, infixExpressions);

   std::vector<Block*> innerIfBlocks;
   std::vector<InfixExpression*>::const_iterator first{infixExpressions.begin()};
   std::vector<InfixExpression*>::const_iterator firstSplit{
      infixExpressions.begin() + returnBlocks.size() / 2};
   std::vector<InfixExpression*> innerInfix{first, firstSplit};
   createIfBlocks(innerInfix, returnBlocks, innerIfBlocks);

   std::vector<Block*> middleIfBlocks;
   // Middle infix has only two elements... end() iterators point one object past the 
   // last object in a vector
   std::vector<InfixExpression*> middleInfix{firstSplit, firstSplit + 2};
   createIfBlocks(middleInfix, innerIfBlocks, middleIfBlocks);

   IfStatement* outerIf{new IfStatement{infixExpressions.back(),
      middleIfBlocks.front(), middleIfBlocks.back()}};
   myBlock->addStatement(outerIf);
}

//------------------------------------------------------------------------------------

void randomizeTree(std::vector<std::string>& returnValues, 
      std::vector<std::string>& ifNames, std::vector<std::string>& ifNumbers)
{
   const unsigned NUMBER_VALUES{8}; // 2^n, n integer >= 0
   const unsigned NUMBER_IFS{7}; // sum from 1 to log(n) of NUMBER_VALUES/2^i
   const unsigned RANGE{20};

   fillRandomVector(NUMBER_VALUES, RANGE, returnValues, true);
   fillRandomVector(NUMBER_IFS, RANGE, ifNumbers);

   // create a sequence of variables v, u, w that goes from left to right, bottom
   // level of if-statements to top. E.g.: {v, v, u, u, u, v, w}
   std::vector<std::string> params{"v", "u", "w"};
   // random number generator for index of params
   Rand_int rnd{0, static_cast<int>(params.size()) - 1}; 
   int index{rnd()};
   ifNames.push_back(params.at(index));
   params.erase(params.begin() + index);
   Rand_int rndTwo{0, static_cast<int>(params.size()) - 1};
   for (int i=0; i<2; ++i) {
      index = rndTwo(); // New index of params
      ifNames.push_back(params.at(index));
   }
   std::string notAllowed{ifNames.at(1)};
   for (int i=0; i<2; ++i)
      ifNames.push_back(*std::find_if_not(params.begin(), params.end(), 
               [notAllowed](std::string s) { return s == notAllowed; }));
   notAllowed = ifNames.at(2);
   for (int i=0; i<2; ++i)
      ifNames.push_back(*std::find_if_not(params.begin(), params.end(), 
               [notAllowed](std::string s) { return s == notAllowed; }));
   std::reverse(ifNames.begin(), ifNames.end()); 
}

//------------------------------------------------------------------------------------

// Fills vec with uniformly distributed integers from -range to range about 0,
// in string form ("0", "1", etc.)
void fillRandomVector(const unsigned numberToFill, const int range, 
      std::vector<std::string>& outputVec, const bool unique)
{
   if (!outputVec.empty()) throw BadSize{};

   Rand_int rnd{-range, range};
   std::vector<int> v;
   while (outputVec.size() < numberToFill) {
      int i{rnd()};
      if (unique) {
         if (std::find(v.begin(), v.end(), i) == v.end()) {
            v.push_back(i);
            outputVec.push_back(std::to_string(i));
         }
      }
      else outputVec.push_back(std::to_string(i));
   }
}

//------------------------------------------------------------------------------------

void printA1A2(JavaPrinter* myPrinter, const std::string& studentNumber)
{
      namespace bfs = boost::filesystem;
      makeDirectories(studentNumber);
      bfs::path studentPath{studentNumber};
      bfs::path se2s03Path{studentPath / "se2s03"};

      // Create if-else tree. First create return blocks:
      std::vector<std::string> returnValues1, testNames1, testNumbers1;

      Block* myBlock1{new Block};
      randomizeTree(returnValues1, testNames1, testNumbers1);
      createIfTreeBlock(returnValues1, testNames1, testNumbers1, myBlock1);

      std::vector<std::string> returnValues2, testNames2, testNumbers2;
      Block* myBlock2{new Block};
      randomizeTree(returnValues2, testNames2, testNumbers2);
      createIfTreeBlock(returnValues2, testNames2, testNumbers2, myBlock2);

      // ...
      std::vector<Parameter> casesParams{Parameter{Type::INT, "v"}, 
         Parameter{Type::INT, "u"}, Parameter{Type::INT, "w"}};
      Boilerplate* myProgram1{createBoilerPlate("se2s03", "A1", myBlock1, 
            "cases", casesParams, Type::INT)};
      Boilerplate* myProgram2{createBoilerPlate("se2s03", "A1", myBlock2, 
            "cases", casesParams, Type::INT)};

      writeToFile(se2s03Path, "A1.java", myPrinter, myProgram1);
      printTests(myPrinter, myProgram1, "A1", "cases", "A1Test", studentPath);
      writeToFile(se2s03Path, "A2.java", myPrinter, myProgram2);
      printTests(myPrinter, myProgram2, "A2", "cases", "A2Test", studentPath);
      if (myProgram1) delete myProgram1;
      if (myProgram2) delete myProgram2;
}

//------------------------------------------------------------------------------------

void createInitialization(std::vector<VarDeclFragment*>& a0a1anFragments, 
      std::vector<VarDeclFragment*>& xyFragments)
{
   if (!a0a1anFragments.empty() || !xyFragments.empty()) throw BadSize{};

   const int RANGE{5};
   Rand_int rnd{-RANGE, RANGE};

   // Create recurrence relation.
   std::vector<std::string> variableNames{"a0", "a1", "an", "x", "y"};
   std::vector<std::string> variableValues;
   
   int value{rnd()};
   while (variableValues.size() < variableNames.size()) {
      while (value == 0) value = rnd();
      variableValues.push_back(std::to_string(value));
      value = rnd();
   }

   // int a0 = 3, a1 = -1, an = 0;
   // int x = 1, y = -3;
   std::vector<Name*> varNamePtrs;
   std::vector<NumberLiteral*> varValPtrs;
   for (unsigned i=0; i<variableNames.size(); ++i) {
      varNamePtrs.push_back(new Name{variableNames.at(i)});
      varValPtrs.push_back(new NumberLiteral{variableValues.at(i)});
   }

   // a0, a1 and an declarations go on one line; x and y declarations go on the next.
   for (int i=0; i<3; ++i)
      a0a1anFragments.push_back(new VarDeclFragment{varNamePtrs.at(i), 
            varValPtrs.at(i), Type::INT});
   for (unsigned i=3; i<varNamePtrs.size(); ++i)
      xyFragments.push_back(new VarDeclFragment{varNamePtrs.at(i), varValPtrs.at(i),
            Type::INT});
   
}

//------------------------------------------------------------------------------------

void createForBlock(Block* forBlock)
{
   // { an = x*a0 + y*a1; a0 = a1; a1 = an; }
   InfixExpression* xTimesa0{new InfixExpression{new Name{"x"}, InfixOperator::TIMES,
      new Name{"a0"}}};
   InfixExpression* yTimesa1{new InfixExpression{new Name{"y"}, InfixOperator::TIMES,
      new Name{"a1"}}};
   InfixExpression* xa0Plusya1{new InfixExpression{xTimesa0, 
      InfixOperator::PLUS, yTimesa1}};
   forBlock->addStatement(new AssignmentStatement{new Name{"an"}, xa0Plusya1});
   forBlock->addStatement(new AssignmentStatement{new Name{"a0"}, new Name{"a1"}});
   forBlock->addStatement(new AssignmentStatement{new Name{"a1"}, new Name{"an"}});
}

//------------------------------------------------------------------------------------

void createRecurrenceBlock(Block* myBlock)
{
   std::vector<VarDeclFragment*> a0a1anFragments;
   std::vector<VarDeclFragment*> xyFragments;
   createInitialization(a0a1anFragments, xyFragments);
   myBlock->addStatement(new VarDeclStatement{a0a1anFragments, Type::INT});
   myBlock->addStatement(new VarDeclStatement{xyFragments, Type::INT});

   // if (n==0) return a0;
   InfixExpression* nEqualsZeroInfix{new InfixExpression{new Name{"n"}, 
      InfixOperator::EQUALS, new NumberLiteral{"0"}}};
   ReturnStatement* nEqualsZeroReturn{new ReturnStatement{new Name{"a0"}}};
   myBlock->addStatement(new IfStatement{nEqualsZeroInfix, nEqualsZeroReturn});
   // if (n==1) return a1;
   InfixExpression* nEqualsOneInfix{new InfixExpression{new Name{"n"}, 
      InfixOperator::EQUALS, new NumberLiteral{"1"}}};
   ReturnStatement* nEqualsOneReturn{new ReturnStatement{new Name{"a1"}}};
   myBlock->addStatement(new IfStatement{nEqualsOneInfix, nEqualsOneReturn});

   // for (int i=2; i<=n; i++) { an = x*a0 + y*a1; a0 = a1; a1 = an; }
   std::vector<VarDeclFragment*> forInit{new VarDeclFragment{new Name{"i"}, 
      new NumberLiteral{"2"}, Type::INT}};
   std::vector<Expression*> forUpdaters{new PostfixExpression{new Name{"i"}, 
      PostfixOperator::INCREMENT}};
   Block* forBlock{new Block};
   createForBlock(forBlock);
   myBlock->addStatement(new ForStatement{forInit, new InfixExpression{new Name{"i"},
         InfixOperator::LESS_EQUALS, new Name{"n"}}, forUpdaters, forBlock});
   // return an;
   myBlock->addStatement(new ReturnStatement{new Name{"an"}});
}

//------------------------------------------------------------------------------------

void printA3(JavaPrinter* myPrinter, const std::string& studentNumber)
{
   namespace bfs = boost::filesystem;
   bfs::path studentPath{studentNumber};
   bfs::path se2s03Path{studentPath / "se2s03"};

   Block* myBlock{new Block};
   createRecurrenceBlock(myBlock);

   std::vector<Parameter> recParams{Parameter{Type::INT, "n"}};
   Boilerplate* myProgram{createBoilerPlate("se2s03", "A3", myBlock,
         "Rec", recParams, Type::INT)};
   writeToFile(se2s03Path, "A3.java", myPrinter, myProgram);
   if (myProgram) delete myProgram;
}

//------------------------------------------------------------------------------------

void printTests(JavaPrinter* myPrinter, Boilerplate* myProgram, 
      const std::string& className, const std::string& methodName, 
      const std::string& name, boost::filesystem::path studentPath)
{
   const unsigned NUMBER_TESTS{205};
   const int TEST_RANGE{100};
   Rand_int rnd{-TEST_RANGE, TEST_RANGE}; // test values in [-100, 100]
   std::vector<std::string> params{"v", "u", "w"};
   TesterBoilerplate* tester{new TesterBoilerplate{"se2s03", className, methodName,
      name}};
   std::string csvFileName{className + ".csv"};
   boost::filesystem::path csvPath{studentPath / csvFileName};
   std::ofstream outFileStream{csvPath.c_str()};
   for (unsigned i=0; i<NUMBER_TESTS; ++i) {
      std::vector<int> randomArgs;
      for (unsigned j=0; j<params.size(); ++j) {
         randomArgs.push_back(rnd());
         outFileStream << randomArgs.back() << ", ";
      }
      ResultFinder myFinder{randomArgs, params};
      myProgram->accept(&myFinder);
      tester->addAssert(new AssertStatement{methodName, randomArgs, 
            std::stoi(myFinder.getResult())});
      outFileStream << myFinder.getResult() << std::endl;
   }
   std::string fileName = name + ".java";
   writeToFile(studentPath, fileName, myPrinter, tester);
   if (tester) delete tester;
}
