#include "AST.h"

void HaskellPrinter::visit(TesterBoilerplate* tester)
{
   *m_os << "import Test.HUnit" << std::endl;
   // oops, capitalized package names
   *m_os << "import " << "Se2s03." << tester->getClassName()
     << std::endl;

   setIndents(0);
   for (unsigned i=0; i<tester->getAsserts().size(); ++i) {
      printIndents();
      *m_os << "test" << i+1 << " = TestCase ("; 
      tester->getAsserts().at(i)->accept(this);
      *m_os << ')' << std::endl;
   }

   *m_os << std::endl;
   *m_os << "tests = TestList [";
   for (int i=0; i< (int) tester->getAsserts().size(); ++i) {
      if (i < (int) tester->getAsserts().size() - 1)
         *m_os << "TestLabel \"test" << i+1 << "\" test" << i+1 << ',' << std::endl
            << "                  ";
      else
         *m_os << "TestLabel \"test" << i+1 << "\" test" << i+1 << ']';
   }
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(Boilerplate* boilerplate)
{
   setIndents(0);
   *m_os << "module " << boilerplate->getName(0) << '.' 
      << boilerplate->getName(1) << std::endl;
   for (Declaration* d : boilerplate->getBodyDeclarations())
      d->accept(this);
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(MethodDeclaration* methodDeclaration)
{
   *m_os << '(' << methodDeclaration->getName() << ") where\n\n"
      << methodDeclaration->getName() << " :: (Integral a) => ";
   if (methodDeclaration->getParamList().size() > 0) {
      for (unsigned i=0; i<methodDeclaration->getParamList().size(); ++i)
         *m_os << "a -> ";
      *m_os << 'a' << std::endl << methodDeclaration->getName() << ' ';
      for (unsigned i=0; i<methodDeclaration->getParamList().size()-1; ++i) {
         const Parameter& p = methodDeclaration->getParamList().at(i);
         *m_os << p.name << ' ';
      }
      const Parameter& p = methodDeclaration->getParamList().back();
      *m_os << p.name << " = do" << std::endl;
   }
   incrementIndents();
   printIndents();
   methodDeclaration->getBody()->accept(this);
   decrementIndents();
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(VarDeclStatement* varDeclStatement)
{
   *m_os << "let ";
   for (unsigned i=0; i<varDeclStatement->getFragments().size(); ++i) {
      varDeclStatement->getFragments().at(i)->accept(this);
      if (i != varDeclStatement->getFragments().size() - 1)
         *m_os << "; ";
   }
   *m_os << std::endl;
   printIndents();
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(AssertStatement* assert)
{
   *m_os << "assertEqual \"for " << assert->getMethodName() << ' ';

   for (unsigned i=0; i<assert->getParameters().size()-1; ++i)
      *m_os << assert->getParameters().at(i) << ' ';
   *m_os << assert->getParameters().back() << ", \" (" << assert->getResult()
      << ") (" << assert->getMethodName() << ' ';

   for (unsigned i=0; i<assert->getParameters().size()-1; ++i)
      *m_os << '(' << assert->getParameters().at(i) << ") ";
   *m_os << '(' << assert->getParameters().back() << "))";
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(Block* block)
{
   for (Statement* statement : block->getStatements())
      statement->accept(this);
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(ReturnStatement* returnStatement)
{
   returnStatement->getExpression()->accept(this);
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(AssignmentStatement* assignmentStatement)
{
   *m_os << '(';
   assignmentStatement->getExpression()->accept(this);
   *m_os << ')';
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(IfStatement* ifStatement)
{
   *m_os << "if ";
   ifStatement->getExpression()->accept(this);
   *m_os << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "then ";
   ifStatement->getThenStatement()->accept(this);

   if (ifStatement->getElseStatement()) {
      *m_os << std::endl; 
      printIndents();
      *m_os << "else ";
      ifStatement->getElseStatement()->accept(this);
      decrementIndents();
   }
   else {
      *m_os << std::endl;
      printIndents();
      *m_os << "else do" << std::endl;
      incrementIndents();
      printIndents();
   }
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(ForStatement* forStatement)
{
   *m_os << "let for i a0 a1 an x y" << std::endl; // cop-out
   incrementIndents();
   incrementIndents();
   incrementIndents();
   printIndents();
   *m_os << "| ";
   forStatement->getExpression()->accept(this);
   *m_os << " = for (i+1) a1 (a0*x + a1*y) (a0*x + a1*y) x y" << std::endl;
   printIndents();
   *m_os << "| otherwise = an" << std::endl;
   decrementIndents();
   decrementIndents();
   decrementIndents();
   printIndents();
   *m_os << "let an = for 2 a0 a1 an x y" << std::endl;
   printIndents();
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(BooleanLiteral* booleanLiteral)
{
   if (booleanLiteral->booleanValue() == true)
      *m_os << "True";
   else *m_os << "False";
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(NumberLiteral* numberLiteral)
{
   std::string token{numberLiteral->getToken()};
   *m_os << token;
}

//------------------------------------------------------------------------------------

// Prefix, rather than infix here :P
void HaskellPrinter::visit(InfixExpression* infixExpression)
{
   infixExpression->getLeftOperand()->accept(this);
   *m_os << ' ' << infixOpToString(infixExpression->getOperator()) << ' ';
   infixExpression->getRightOperand()->accept(this);
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(PostfixExpression* postfixExpression)
{
}

//------------------------------------------------------------------------------------

void HaskellPrinter::visit(VarDeclFragment* varDeclFragment)
{
   varDeclFragment->getLeftOperand()->accept(this);
   *m_os << " = ";
   varDeclFragment->getRightOperand()->accept(this);
}
