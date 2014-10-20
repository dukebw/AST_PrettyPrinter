#
# A simple makefile for compiling C++ programs

CXXFLAGS = -std=c++11 -Wall -pedantic
CXX = g++
SOURCES = test_print_AST.cpp AST.cpp JavaPrinter.cpp ResultFinder.cpp \
			 JavaScriptPrinter.cpp SchemePrinter.cpp MissingBracket.cpp \
			 HaskellPrinter.cpp
OBJS = ${SOURCES:.cpp=.o}
TARGETS = test_print_AST
LINK = -lboost_filesystem -lboost_system

$(TARGETS) : $(OBJS)
	$(CXX) -o $(TARGETS) $(OBJS) $(LINK)

# A rule to build .o file out of a .cpp file
%.o: %.cpp AST.h
	$(CXX) $(CXXFLAGS) -o $@ -c $< 

# A rule to clean all the intermediates and targets
clean:
	rm -rf $(TARGETS) $(OBJS)
