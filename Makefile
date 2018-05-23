SRC=$(wildcard *.cpp)
TESTS=$(wildcard tests-*.cpp)
CPPFLAGS += -std=c++17 -Wall -Wextra -MD -MP -ggdb -DDEBUG $(shell pkg-config --cflags sdl2)
LDFLAGS += -lstdc++ $(shell pkg-config --libs sdl2)

default: main

main: main.o chip8.o
	$(CXX) $+ -o $@

tests: $(TESTS:.cpp=.o) chip8.o
	$(CXX) $+ -o $@

-include $(SRC:%.cpp=%.d)
