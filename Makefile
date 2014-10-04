#
# A simple makefile for compiling C++ programs

CXXFLAGS = -std=c++11 -Wall
CXX = g++
SOURCES = test_print_AST.cpp AST.cpp
OBJS = ${SOURCES:.cpp=.o}
TARGETS = test_print_AST

$(TARGETS) : $(OBJS)
	$(CXX) -o $(TARGETS) $(OBJS)

# A rule to build .o file out of a .cpp file
%.o: %.cpp AST.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

# A rule to clean all the intermediates and targets
clean:
	rm -rf $(TARGETS) $(OBJS)