#include "AST.h"
#include <cstdlib>

int main()
try {
   std::string studentNumberFile{"STUDENT_NUMBERS"};
   std::vector<std::string> studentNumbers;
   readStudentNumbers(studentNumberFile, studentNumbers);

   // visitor
   JavaPrinter myJavaPrinter;
   JavaScriptPrinter myJsPrinter;
   SchemePrinter myScmPrinter;
   HaskellPrinter myHaskellPrinter;
   for (const std::string& s : studentNumbers) {
      printA1A2(&myJavaPrinter, s, "Java");
      printA3(&myJavaPrinter, s, "Java");

      printA1A2(&myJsPrinter, s, "JavaScript");
      printA3(&myJsPrinter, s, "JavaScript");

      printA1A2(&myScmPrinter, s, "Scheme");
      printA3(&myScmPrinter, s, "Scheme");

      printA1A2(&myHaskellPrinter, s, "Haskell");
      printA3(&myHaskellPrinter, s, "Haskell");
   }
}
catch (BadArgument) {
   std::cerr << "Unexpected argument found." << std::endl;
}
catch (BadSize) {
   std::cerr << "Range error, or other size error." << std::endl;
}
catch (BadPath) {
   std::cerr << "Path error." << std::endl;
}
catch (std::exception& e) {
   std::cerr << e.what() << std::endl;
}
catch (...) {
   std::cerr << "Some error" << std::endl;
}
