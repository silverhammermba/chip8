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

#define CHIP8_OP(NAME, N, X, Y) std::make_tuple(&Chip8:: op_ ## NAME, N, X, Y)

TEST_CASE("Opcodes are decodable", "[chip8]")
{
	REQUIRE(Chip8::decode_opcode(0x00e0) == CHIP8_OP(clear, 0, 0, 0));
	REQUIRE(Chip8::decode_opcode(0x00ee) == CHIP8_OP(ret, 0, 0, 0));

	REQUIRE(Chip8::decode_opcode(0x1bed) == CHIP8_OP(goto, 0xbed, 0, 0));

	REQUIRE(Chip8::decode_opcode(0x2b0b) == CHIP8_OP(call, 0xb0b, 0, 0));

	REQUIRE(Chip8::decode_opcode(0x3a69) == CHIP8_OP(if_eq, 0x69, 0xa, 0));

	REQUIRE(Chip8::decode_opcode(0x4b42) == CHIP8_OP(if_ne, 0x42, 0xb, 0));

	REQUIRE(Chip8::decode_opcode(0x5cd0) == CHIP8_OP(if_cmp, 0, 0xc, 0xd));

	REQUIRE(Chip8::decode_opcode(0x620a) == CHIP8_OP(store, 0x0a, 0x2, 0));

	REQUIRE(Chip8::decode_opcode(0x7f21) == CHIP8_OP(add, 0x21, 0xf, 0));

	REQUIRE(Chip8::decode_opcode(0x8120) == CHIP8_OP(set, 0, 0x1, 0x2));
	REQUIRE(Chip8::decode_opcode(0x8fe1) == CHIP8_OP(or, 0, 0xf, 0xe));
	REQUIRE(Chip8::decode_opcode(0x84c2) == CHIP8_OP(and, 0, 0x4, 0xc));
	REQUIRE(Chip8::decode_opcode(0x8903) == CHIP8_OP(xor, 0, 0x9, 0x0));
	REQUIRE(Chip8::decode_opcode(0x8134) == CHIP8_OP(madd, 0, 0x1, 0x3));
	REQUIRE(Chip8::decode_opcode(0x8875) == CHIP8_OP(sub, 0, 0x8, 0x7));
	REQUIRE(Chip8::decode_opcode(0x8bc6) == CHIP8_OP(shiftr, 0, 0xb, 0xc));
	REQUIRE(Chip8::decode_opcode(0x8cd7) == CHIP8_OP(rsub, 0, 0xc, 0xd));
	REQUIRE(Chip8::decode_opcode(0x8e3e) == CHIP8_OP(shiftl, 0, 0xe, 0x3));

	REQUIRE(Chip8::decode_opcode(0x9460) == CHIP8_OP(if_ncmp, 0, 0x4, 0x6));

	REQUIRE(Chip8::decode_opcode(0xadad) == CHIP8_OP(save, 0xdad, 0, 0));

	REQUIRE(Chip8::decode_opcode(0xb0ff) == CHIP8_OP(jmp, 0x0ff, 0, 0));

	REQUIRE(Chip8::decode_opcode(0xcdee) == CHIP8_OP(rand, 0xee, 0xd, 0));

	REQUIRE(Chip8::decode_opcode(0xd2eb) == CHIP8_OP(disp, 0xb, 0x2, 0xe));

	REQUIRE(Chip8::decode_opcode(0xe39e) == CHIP8_OP(press, 0, 0x3, 0));
	REQUIRE(Chip8::decode_opcode(0xeca1) == CHIP8_OP(release, 0, 0xc, 0));

	REQUIRE(Chip8::decode_opcode(0xf707) == CHIP8_OP(getdel, 0, 0x7, 0));
	REQUIRE(Chip8::decode_opcode(0xf80a) == CHIP8_OP(wait, 0, 0x8, 0));
	REQUIRE(Chip8::decode_opcode(0xf915) == CHIP8_OP(setdel, 0, 0x9, 0));
	REQUIRE(Chip8::decode_opcode(0xf318) == CHIP8_OP(setsnd, 0, 0x3, 0));
	REQUIRE(Chip8::decode_opcode(0xf41e) == CHIP8_OP(inc, 0, 0x4, 0));
	REQUIRE(Chip8::decode_opcode(0xf529) == CHIP8_OP(font, 0, 0x5, 0));
	REQUIRE(Chip8::decode_opcode(0xf633) == CHIP8_OP(deci, 0, 0x6, 0));
	REQUIRE(Chip8::decode_opcode(0xfa55) == CHIP8_OP(dump, 0, 0xa, 0));
	REQUIRE(Chip8::decode_opcode(0xfc65) == CHIP8_OP(load, 0, 0xc, 0));
}

TEST_CASE("Invalid opcodes decode to null", "[chip8]")
{
	auto nullop = std::make_tuple((Chip8::opfn_t)nullptr, 0, 0, 0);
	REQUIRE(Chip8::decode_opcode(0x00f0) == nullop);
	REQUIRE(Chip8::decode_opcode(0x5243) == nullop);
	REQUIRE(Chip8::decode_opcode(0x8229) == nullop);
	REQUIRE(Chip8::decode_opcode(0x987d) == nullop);
	REQUIRE(Chip8::decode_opcode(0xe1fa) == nullop);
	REQUIRE(Chip8::decode_opcode(0xf235) == nullop);
}

TEST_CASE("Opcode clear 00E0", "[chip8]")
{
	Chip8 chip8;
}

TEST_CASE("Opcode ret 00EE", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode goto 1NNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode call 2NNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode if_eq 3XNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode if_ne 4XNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode if_cmp 5XY0", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode store 6XNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode add 7XNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode set 8XY0", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode or 8XY1", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode and 8XY2", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode xor 8XY3", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode madd 8XY4", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode sub 8XY5", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode shiftr 8XY6", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode rsub 8XY7", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode shiftl 8XYE", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode if_ncmp 9XY0", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode save ANNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode jmp BNNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode rand CXNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode disp DXYN", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode press EX9E", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode release EXA1", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode getdel FX07", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode wait FX0A", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode setdel FX15", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode setsnd FX18", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode inc FX1E", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode font FX29", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode deci FX33", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode dump FX55", "[chip8]")
{
	// TODO
}

TEST_CASE("Opcode load FX65", "[chip8]")
{
	// TODO
}
