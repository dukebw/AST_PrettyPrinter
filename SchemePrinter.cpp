#include "AST.h"

void SchemePrinter::visit(TesterBoilerplate* tester)
{
   *m_os << "(include \"assert.scm\")" << std::endl;
   *m_os << "(include \"" << tester->getPackageName() << '/' << tester->getClassName()
     << ".scm\")" << std::endl;
   setIndents(0);
   *m_os << "(and" << std::endl;
   incrementIndents();
   for (AssertStatement* as : tester->getAsserts()) {
      printIndents();
      as->accept(this);
   }
   printIndents();
   *m_os << "(print \"Tests passed!\\n\"))" << std::endl;
   decrementIndents();
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(Boilerplate* boilerplate)
{
   setIndents(0);
   for (Declaration* d : boilerplate->getBodyDeclarations())
      d->accept(this);
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(MethodDeclaration* methodDeclaration)
{
   printIndents();
   *m_os << "(define (" << methodDeclaration->getName() << ' ';
   if (methodDeclaration->getParamList().size() > 0) {
      for (unsigned i=0; i<methodDeclaration->getParamList().size()-1; ++i) {
         const Parameter& p = methodDeclaration->getParamList().at(i);
         *m_os << p.name << ' ';
      }
      const Parameter& p = methodDeclaration->getParamList().back();
      *m_os << p.name;
   }
   *m_os << ')' << std::endl;
   incrementIndents();

   methodDeclaration->getBody()->accept(this);
   *m_os << ')' << std::endl;
   decrementIndents();
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(VarDeclStatement* varDeclStatement)
{
   // Note: unbalanced bracket. This must be balanced by the function block.
   printIndents();
   *m_os << "(let (";
   for (unsigned i=0; i<varDeclStatement->getFragments().size(); ++i) {
      varDeclStatement->getFragments().at(i)->accept(this);
      if (i != varDeclStatement->getFragments().size() - 1)
         *m_os << ' ';
   }
   *m_os << ')' << std::endl;
   incrementIndents();
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(AssertStatement* assert)
{
   *m_os << "(assert \"(" << assert->getMethodName() << ' ';

   for (unsigned i=0; i<assert->getParameters().size()-1; ++i)
      *m_os << assert->getParameters().at(i) << ' ';
   *m_os << assert->getParameters().back() << ") must be " << assert->getResult()
      << "\" (= (" << assert->getMethodName() << ' ';

   for (unsigned i=0; i<assert->getParameters().size()-1; ++i)
      *m_os << assert->getParameters().at(i) << ' ';
   *m_os << assert->getParameters().back() << ") " << assert->getResult() << "))"
      << std::endl;
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(Block* block)
{
   for (Statement* statement : block->getStatements())
      statement->accept(this);

   MissingBracket mb;
   mb.setOutStream(*m_os);
   for (Statement* statement : block->getStatements())
      statement->accept(&mb);
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(ReturnStatement* returnStatement)
{
   printIndents();
   returnStatement->getExpression()->accept(this);
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(AssignmentStatement* assignmentStatement)
{
   printIndents();
   *m_os << "(set! ";
   assignmentStatement->getName()->accept(this);
   *m_os << ' ';
   assignmentStatement->getExpression()->accept(this);
   *m_os << ')' << std::endl;
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(IfStatement* ifStatement)
{
   printIndents();
   *m_os << "(if ";
   ifStatement->getExpression()->accept(this);
   *m_os << std::endl;
   incrementIndents();
   ifStatement->getThenStatement()->accept(this);

   if (ifStatement->getElseStatement()) {
      *m_os << std::endl;
      ifStatement->getElseStatement()->accept(this);
      decrementIndents();
      *m_os << ')'; 
   }
   else {
      *m_os << std::endl;
      printIndents();
      *m_os << "(begin" << std::endl;
      incrementIndents();
   }
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(ForStatement* forStatement)
{
   printIndents();
   *m_os << "(let for";
   if (!forStatement->getInitializers().empty())
      *m_os << '(';
   for (Expression* e : forStatement->getInitializers()) e->accept(this);
   *m_os << ')' << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "(if ";
   forStatement->getExpression()->accept(this);
   *m_os << std::endl;
   incrementIndents();
   printIndents();
   *m_os << "(begin" <<std::endl;
   incrementIndents();
   forStatement->getBody()->accept(this);
   printIndents();
   *m_os << "(for ";
   for (Expression* e : forStatement->getUpdaters()) e->accept(this);
   *m_os << "))))";
   *m_os << std::endl;
   for (int i=0; i<3; ++i) decrementIndents();
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(BooleanLiteral* booleanLiteral)
{
   if (booleanLiteral->booleanValue() == true)
      *m_os << "#t";
   else *m_os << "#f";
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(NumberLiteral* numberLiteral)
{
   std::string token{numberLiteral->getToken()};
   *m_os << token;
}

//------------------------------------------------------------------------------------

// Prefix, rather than infix here :P
void SchemePrinter::visit(InfixExpression* infixExpression)
{
   *m_os << '(';
   *m_os << infixOpToStringScm(infixExpression->getOperator()) << ' ';
   infixExpression->getLeftOperand()->accept(this);
   *m_os << ' ';
   infixExpression->getRightOperand()->accept(this);
   *m_os << ')';
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(PostfixExpression* postfixExpression)
{
   *m_os << '(' << postfixOpToStringScm(postfixExpression->getOperator()) << ' ';
   postfixExpression->getLeftOperand()->accept(this);
   *m_os << " 1)";
}

//------------------------------------------------------------------------------------

void SchemePrinter::visit(VarDeclFragment* varDeclFragment)
{
   *m_os << '(';
   varDeclFragment->getLeftOperand()->accept(this);
   *m_os << ' ';
   varDeclFragment->getRightOperand()->accept(this);
   *m_os << ')';
}
