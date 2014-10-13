#include "AST.h"

int main()
try {
   std::string studentNumberFile{"STUDENT_NUMBERS"};
   std::vector<std::string> studentNumbers;
   readStudentNumbers(studentNumberFile, studentNumbers);

   // visitor
   JavaPrinter myPrinter;
   for (const std::string& s : studentNumbers) {
      printA1A2(&myPrinter, s);
      printA3(&myPrinter, s);
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
