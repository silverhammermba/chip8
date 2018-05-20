SRC=$(wildcard *.cpp)
TESTS=$(wildcard tests-*.cpp)
CPPFLAGS += -Wall -MD -MP

default: main

tests: $(TESTS:.cpp=.o)
	$(CXX) $+ -o $@

-include $(SRC:%.cpp=%.d)
