#include "AST.h"

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
   m_res = std::to_string(m_compareVal);
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(AssignmentStatement* assignmentStatement)
{
   assignmentStatement->getName()->accept(this);
   // Check that we're assigning to a variable already in the symbol table:
   auto result = std::find(m_inNames.begin(), m_inNames.end(), m_compareName);
   if (result == m_inNames.end()) throw BadArgument{};

   // Save the to-be-assigned-to name
   std::string assignTo{m_compareName};
   // Evaluate the right-hand expression:
   assignmentStatement->getExpression()->accept(this);
   for (unsigned i=0; i<m_inNames.size(); ++i)
      if (m_inNames.at(i) == assignTo)
         m_in.at(i) = m_compareVal;
}

//------------------------------------------------------------------------------------

void ResultFinder::visit(IfStatement* ifStatement)
{
   ifStatement->getExpression()->accept(this);
   if (m_compare)
      ifStatement->getThenStatement()->accept(this);
   else if (ifStatement->getElseStatement())
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
   if (result != m_inNames.end()) {
      for (unsigned i=0; i<m_inNames.size(); ++i)
         if (m_inNames.at(i) == m_compareName)
            m_compareVal = m_in.at(i);
   }
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
   int tempVal{m_compareVal};
   InfixOperator tempOp{m_op};
   infixExpression->getRightOperand()->accept(this);
   switch (tempOp) {
      case InfixOperator::LESS_EQUALS:
         if (tempVal <= m_compareVal) m_compare = true;
         else m_compare = false;
         break;
      case InfixOperator::EQUALS:
         if (tempVal == m_compareVal) m_compare = true;
         else m_compare = false;
         break;
      case InfixOperator::PLUS:
         m_compareVal += tempVal;
         break;
      case InfixOperator::TIMES:
         m_compareVal *= tempVal;
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
