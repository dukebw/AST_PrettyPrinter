#include "AST.h"

// Right now main() does the job of building a test AST. This process is 2 pages
// and therefore much too long. It should be split into functions (perhaps one to
// return each successive level of if-statements).
int main()
try {
   // visitor
   JavaPrinter myPrinter;
   const int MAX_RETURN_VALUES{8};

   // Create if-else tree. First create return statements:
   std::vector<NumberLiteral*> returnNumbers;
   for (int i=0; i<MAX_RETURN_VALUES; ++i)
      returnNumbers.push_back(new NumberLiteral{"II"});
   std::vector<ReturnStatement*> returnStatements;
   for (unsigned i=0; i<returnNumbers.size(); ++i)
      returnStatements.push_back(new ReturnStatement{returnNumbers.at(i)});
   std::vector<Block*> returnBlocks;
   for (int i=0; i<MAX_RETURN_VALUES; ++i) {
      returnBlocks.push_back(new Block);
      returnBlocks.at(i)->addStatement(returnStatements.at(i));
   }

   // Next create infix expressions:
   std::vector<std::string> testNames{"v", "u", "w", "w", "w", "u", "u"};
   std::vector<Name*> variables;
   for (unsigned i=0; i<testNames.size(); ++i)
      variables.push_back(new Name{testNames.at(i)});
   std::vector<NumberLiteral*> ifNumbers;
   for (unsigned i=0; i<testNames.size(); ++i)
      ifNumbers.push_back(new NumberLiteral{"II"});
   std::vector<InfixExpression*> infixExpressions;
   for (int i=ifNumbers.size()-1; i>=0; --i)
      infixExpressions.push_back(new InfixExpression{variables.at(i),
            InfixOperator::LESS_EQUALS, ifNumbers.at(i)});

   // Create inner if statements:
   std::vector<IfStatement*> innerIfs;
   std::vector<Block*> innerIfBlocks;
   for (int i=0; i<MAX_RETURN_VALUES/2; ++i) {
      innerIfs.push_back(new IfStatement{infixExpressions.at(i),
            returnBlocks.at(2*i), returnBlocks.at(2*i+1)});
      innerIfBlocks.push_back(new Block);
      innerIfBlocks.at(i)->addStatement(innerIfs.at(i));
   }

   // Create middle if statements
   std::vector<IfStatement*> middleIfs;
   std::vector<Block*> middleIfBlocks;
   for (unsigned i=0; i<innerIfs.size()/2; ++i) {
      middleIfs.push_back(new IfStatement{infixExpressions.at(innerIfs.size()+i),
               innerIfBlocks.at(2*i), innerIfBlocks.at(2*i+1)});
      middleIfBlocks.push_back(new Block);
      middleIfBlocks.at(i)->addStatement(middleIfs.at(i));
   }

   IfStatement* outerIf{new IfStatement{infixExpressions.back(),
      middleIfBlocks.front(), middleIfBlocks.back()}};

   // ...
   Block* myBlock{new Block};
   myBlock->addStatement(outerIf);
   std::vector<Parameter> casesParams;
   casesParams.push_back(Parameter{Type::INT, "v"});
   casesParams.push_back(Parameter{Type::INT, "u"});
   casesParams.push_back(Parameter{Type::INT, "w"});
   MethodDeclaration* myMethod{new MethodDeclaration{myBlock, "cases", 
      casesParams, Type::INT}};
   ClassDeclaration* myClass{new ClassDeclaration{"A1"}};
   myClass->addDeclaration(myMethod);
   PackageDeclaration* myPackage{new PackageDeclaration{"se2s03", myClass}};

   // print
   myPackage->accept(&myPrinter);
}
catch (std::exception& e) {
   std::cerr << e.what() << std::endl;
}
catch (...) {
   std::cerr << "Some error" << std::endl;
}
