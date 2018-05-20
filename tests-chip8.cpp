#include <catch/catch.hpp>
#include "chip8.hpp"

TEST_CASE("Opcodes are retrievable", "[chip8]")
{
	std::array<uint8_t, Chip8::memory_size> memory;
	memory[0] = 0xde;
	memory[1] = 0xad;

	REQUIRE(Chip8::get_opcode(memory, 0) == 0xdead);
}
