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

PackageDeclaration::~PackageDeclaration()
{
   if (m_classDecl) delete m_classDecl;
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

void JavaPrinter::visit(PackageDeclaration* packageDeclaration)
{
   printIndents();
   m_os << "package " << packageDeclaration->getName() << ';' << std::endl;
   packageDeclaration->getClassDeclaration()->accept(this);
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(ClassDeclaration* classDeclaration)
{
   printIndents();
   m_os << "public class " << classDeclaration->getName() << " {" << std::endl;
   incrementIndents();
   for (Declaration* d : classDeclaration->getBodyDeclarations())
      d->accept(this);   

   decrementIndents();
   m_os << '}' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(MethodDeclaration* methodDeclaration)
{
   printIndents();
   m_os << "public " << typeToString(methodDeclaration->getReturnType()) << ' '
      << methodDeclaration->getName() << '(';
   
   // If statement formats argument list so it's (int x, int y, char z, ...),
   // or "()" if there are no arguments
   if (methodDeclaration->getParamList().size() > 0) {
      using size_type = std::vector<Parameter>::size_type;
      for (size_type i = 0; i<methodDeclaration->getParamList().size()-1; ++i) {
            const Parameter& p = methodDeclaration->getParamList().at(i);
            m_os << typeToString(p.type) << ' ' << p.name << ", ";
      }

      const Parameter& p = methodDeclaration->getParamList().back();
      m_os << typeToString(p.type) << ' ' << p.name << ") ";
   }
   else m_os << ") ";

   methodDeclaration->getBody()->accept(this);
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(Block* block)
{
   m_os << '{' << std::endl;

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
   m_os << '}' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(ReturnStatement* returnStatement)
{
   m_os << "return ";
   returnStatement->getExpression()->accept(this);
   m_os << ';' << std::endl;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(IfStatement* ifStatement)
{
   m_os << "if (";
   ifStatement->getExpression()->accept(this); // print expression
   m_os << ") ";
   ifStatement->getThenStatement()->accept(this);
   
   if (ifStatement->getElseStatement()) {
      printIndents();
      m_os << "else ";
      ifStatement->getElseStatement()->accept(this);
   }
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(BooleanLiteral* booleanLiteral)
{
   if (booleanLiteral->booleanValue() == true)
      m_os << "true";
   else m_os << "false";
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(NumberLiteral* numberLiteral)
{
   std::string token{numberLiteral->getToken()};
   m_os << token;
}

//------------------------------------------------------------------------------------

void JavaPrinter::visit(InfixExpression* infixExpression)
{
   infixExpression->getLeftOperand()->accept(this);
   m_os << ' ' << infixOpToString(infixExpression->getOperator()) << ' ';
   infixExpression->getRightOperand()->accept(this);
}

//------------------------------------------------------------------------------------

void JavaPrinter::printIndents() const
{
   for (int i=0; i<m_indents; ++i)
      m_os << "   ";
}
