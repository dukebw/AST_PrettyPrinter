#include "AST.h"

int main()
try {
   JavaPrinter testPrinter;
   std::vector<Statement*> testStatements;
   Block testBlock{testStatements};
   testBlock.accept(&testPrinter);
}
catch (std::exception& e) {
   std::cerr << e.what() << std::endl;
}
catch (...) {
   std::cerr << "Some error" << std::endl;
}
