#include "AST.h"

void JavaPrinter::visit(TesterBoilerplate* tester)
{  // Temporary fix
   *m_os << "import static org.junit.Assert.*;" << std::endl;
   *m_os << "import org.junit.BeforeClass;" << std::endl;
   *m_os << "import org.junit.Test;" << std::endl;
   *m_os << "import " << tester->getPackageName() << '.' << tester->getClassName()
      << ';' << std::endl;
   *m_os << "public class " << tester->getName() << " {" << std::endl;
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
      *m_os << typeToString(p.type) << ' ' << p.name; 
   }
   *m_os << ") ";

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
   *m_os << " = ";
   varDeclFragment->getRightOperand()->accept(this);
}
