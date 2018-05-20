default: main

TESTS=$(wildcard tests-*.cpp)

tests: $(TESTS:.cpp=.o)
	$(CXX) $+ -o $@
