SRC=$(wildcard *.cpp)
TESTS=$(wildcard tests-*.cpp)
CPPFLAGS += -std=c++17 -Wall -Wextra -MD -MP -ggdb -DDEBUG

default: main

main: main.o chip8.o
	$(CXX) $+ -o $@

tests: $(TESTS:.cpp=.o) chip8.o
	$(CXX) $+ -o $@

-include $(SRC:%.cpp=%.d)
