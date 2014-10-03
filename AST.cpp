#include "AST.h"

//------------------------------------------------------------------------------------

void JavaPrinter::printIndents() const
{
   for (int i=0; i<m_indents; ++i)
      std::cout << '\t';
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(const Block& block)
{
   std::cout << '{' << std::endl;

   incrementIndents();
   // print all the statements in the block (if any)
   for (unique_ptr<Statement> const statement : block.getStatements())
      statement->accept(this);
   decrementIndents();

   printIndents();
   std::cout << '}' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(const ReturnStatement& returnStatement)
{
   printIndents();
   std::cout << "return ";
   returnStatement.getExpression()->accept(this);
   std::cout << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(const IfStatement& ifStatement)
{
   printIndents();
   std::cout << "if (";
   ifStatement.getExpression()->accept(this); // print expression
   std::cout << ')';
   incrementIndents();
   ifStatement.getThenStatement()->accept(this);
   
   if (ifStatement.getElseStatement()) {
      printIndents();
      std::cout << "else ";
      ifStatement.getElseStatement()->accept(this);
   }
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(const BooleanLiteral& booleanLiteral)
{
   if (booleanLiteral.booleanValue == true)
      std::cout << "true";
   else std::cout << "false";
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(const NumberLiteral& numberLiteral)
{
   std::string token{numberLiteral.getToken()};
   std::cout << token;
}
