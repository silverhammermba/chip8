#include <catch/catch.hpp>
#include "chip8.hpp"

TEST_CASE("Opcodes are retrievable", "[chip8]")
{
	std::array<uint8_t, Chip8::memory_size> memory;
	memory[0] = 0xde;
	memory[1] = 0xad;
	memory[2] = 0xbe;
	memory[3] = 0xef;

	REQUIRE(Chip8::get_opcode(memory, 0) == 0xdead);
	REQUIRE(Chip8::get_opcode(memory, 1) == 0xadbe);
	REQUIRE(Chip8::get_opcode(memory, 2) == 0xbeef);
	REQUIRE(Chip8::get_opcode(memory, 3) == 0xef00);
}

#define OP(name, arg) std::make_tuple(&Chip8:: name, (arg))

TEST_CASE("Opcodes are decodable", "[chip8]")
{
	REQUIRE(Chip8::decode_opcode(0x00e0) == OP(clear_screen, 0));
}
