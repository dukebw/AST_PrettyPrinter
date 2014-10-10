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
      case InfixOperator::TIMES:
         return "*";
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

void ResultFinder::visit(IfStatement* ifStatement)
{
   ifStatement->getExpression()->accept(this);
   if (m_compare)
      ifStatement->getThenStatement()->accept(this);
   else
      ifStatement->getElseStatement()->accept(this);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(Name* name)
{
   m_compareName = name->getName();
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
   infixExpression->getLeftOperand()->accept(this);
   for (unsigned i=0; i<m_in.size(); ++i)
      if (m_inNames.at(i) == m_compareName) {
         m_op = infixExpression->getOperator();
         infixExpression->getRightOperand()->accept(this);
         switch (m_op) {
            case InfixOperator::LESS_EQUALS:
               if (m_in.at(i) <= m_compareVal) m_compare = true;
               else m_compare = false;
               break;
            case InfixOperator::EQUALS:
               if (m_in.at(i) == m_compareVal) m_compare = true;
               else m_compare = false;
               break;
            default:
               throw BadComparison{};
         }
      }
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
