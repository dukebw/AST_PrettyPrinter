#include "AST.h"

void MissingBracket::visit(IfStatement* ifStatement) 
{ 
   if (ifStatement->getElseStatement() == nullptr)
      *m_os << "))";
}
