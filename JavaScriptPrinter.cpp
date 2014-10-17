#include "AST.h"

void JavaScriptPrinter::visit(TesterBoilerplate* tester)
{
   *m_os << "load(\"assert.js\");" << std::endl;
   *m_os << "load(\"" << tester->getPackageName() << '/' << tester->getClassName()
     << ".js\");" << std::endl;
   *m_os << "var " << tester->getMethodName() << "Test = function () {" << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "try {" << std::endl;
   incrementIndents();
   for (AssertStatement* as : tester->getAsserts()) {
      printIndents();
      as->accept(this);
   }
   printIndents();
   *m_os << "print('Tests passed!')" << std::endl;
   decrementIndents();
   *m_os << "} catch (e) {" << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "print(e.name + ': ' + e.message);" << std::endl;
   decrementIndents();
   printIndents();
   *m_os << '}' << std::endl;
   decrementIndents();
   printIndents();
   *m_os << '}' << std::endl << std::endl;
   printIndents();
   *m_os << tester->getMethodName() << "Test();" << std::endl;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(Boilerplate* boilerplate)
{
   for (Declaration* d : boilerplate->getBodyDeclarations())
      d->accept(this);
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(MethodDeclaration* methodDeclaration)
{
   printIndents();
   *m_os << "function " << methodDeclaration->getName() << '(';
   if (methodDeclaration->getParamList().size() > 0) {
      for (unsigned i=0; i<methodDeclaration->getParamList().size()-1; ++i) {
         const Parameter& p = methodDeclaration->getParamList().at(i);
         *m_os << p.name << ", ";
      }

      const Parameter& p = methodDeclaration->getParamList().back();
      *m_os << ' ' << p.name;
   }
   *m_os << ") ";

   methodDeclaration->getBody()->accept(this);
   *m_os << std::endl;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(VarDeclStatement* varDeclStatement)
{
   *m_os << "var ";
   for (unsigned i=0; i<varDeclStatement->getFragments().size(); ++i) {
      varDeclStatement->getFragments().at(i)->accept(this);
      if (i != varDeclStatement->getFragments().size() - 1)
         *m_os << ", ";
   }
   *m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(AssertStatement* assert)
{
   // assert(Rec(2) === 6, "Rec(2) must be 6");
   *m_os << "assert(" << assert->getMethodName() << '(';
   printIntVector(assert->getParameters());
   *m_os << ") === " << assert->getResult() << ", \"" << assert->getMethodName()
      << '(';
   printIntVector(assert->getParameters());
   *m_os << ") must be " << assert->getResult() << "\");" << std::endl;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(Block* block)
{
   *m_os << '{' << std::endl;

   incrementIndents();
   for (Statement* statement : block->getStatements()) {
      printIndents();
      statement->accept(this);
   }
   decrementIndents();

   printIndents();
   *m_os << '}';
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(ReturnStatement* returnStatement)
{
   *m_os << "return ";
   returnStatement->getExpression()->accept(this);
   *m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(AssignmentStatement* assignmentStatement)
{
   assignmentStatement->getName()->accept(this);
   *m_os << " = ";
   assignmentStatement->getExpression()->accept(this);
   *m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(IfStatement* ifStatement)
{
   *m_os << "if (";
   ifStatement->getExpression()->accept(this);
   *m_os << ") ";
   ifStatement->getThenStatement()->accept(this);

   if (ifStatement->getElseStatement()) {
      *m_os << " else ";
      ifStatement->getElseStatement()->accept(this);
      *m_os << std::endl;
   }
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(ForStatement* forStatement)
{
   *m_os << " for (";
   if (!forStatement->getInitializers().empty())
      *m_os << "var ";
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

void JavaScriptPrinter::visit(BooleanLiteral* booleanLiteral)
{
   if (booleanLiteral->booleanValue() == true)
      *m_os << "true";
   else *m_os << "false";
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(NumberLiteral* numberLiteral)
{
   std::string token{numberLiteral->getToken()};
   *m_os << token;
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(InfixExpression* infixExpression)
{
   infixExpression->getLeftOperand()->accept(this);
   *m_os << ' ' << infixOpToStringJs(infixExpression->getOperator()) << ' ';
   infixExpression->getRightOperand()->accept(this);
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(PostfixExpression* postfixExpression)
{
   postfixExpression->getLeftOperand()->accept(this);
   *m_os << postfixOpToString(postfixExpression->getOperator());
}

//------------------------------------------------------------------------------------

void JavaScriptPrinter::visit(VarDeclFragment* varDeclFragment)
{
   varDeclFragment->getLeftOperand()->accept(this);
   *m_os << " = ";
   varDeclFragment->getRightOperand()->accept(this);
}
