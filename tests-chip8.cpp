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

TEST_CASE("Op clear 00E0", "[chip8]")
{
	// TODO
}

TEST_CASE("Op ret 00EE", "[chip8]")
{
	Chip8 chip8;

	chip8.op_call(0xbad, 0, 0);
	chip8.op_call(0xdad, 0, 0);

	chip8.op_ret(0, 0, 0);
	REQUIRE(chip8.get_program_counter() == 0xbad);
	chip8.op_ret(0, 0, 0);
	REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);

	// TODO test when stack is empty?
}

TEST_CASE("Op goto 1NNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_goto(0x2ad, 0, 0);
	REQUIRE(chip8.get_program_counter() == 0x2ad);
	chip8.op_goto(0xdad, 0, 0);
	REQUIRE(chip8.get_program_counter() == 0xdad);
}

TEST_CASE("Op call 2NNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_call(0xbad, 0, 0);
	REQUIRE(chip8.get_program_counter() == 0xbad);
	chip8.op_call(0xdad, 0, 0);
	REQUIRE(chip8.get_program_counter() == 0xdad);
}

TEST_CASE("Op if_eq 3XNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(12, 1, 0);

	SECTION("when equal")
	{
		chip8.op_if_eq(12, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 2);
		chip8.op_if_eq(12, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
	}
	SECTION("when unequal")
	{
		chip8.op_if_eq(50, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);
	}
}

TEST_CASE("Op if_ne 4XNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(12, 1, 0);

	SECTION("when unequal")
	{
		chip8.op_if_ne(10, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 2);
		chip8.op_if_ne(50, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
	}
	SECTION("when equal")
	{
		chip8.op_if_ne(12, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);
	}
}

TEST_CASE("Op if_cmp 5XY0", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(10, 1, 0);
	chip8.op_store(10, 2, 0);
	chip8.op_store(20, 3, 0);

	SECTION("when equal")
	{
		chip8.op_if_cmp(0, 1, 2);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 2);
		chip8.op_if_cmp(0, 5, 6);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
	}
	SECTION("when unequal")
	{
		chip8.op_if_cmp(0, 1, 3);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);
	}
}

TEST_CASE("Op store 6XNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(20, 4, 0);
	REQUIRE(chip8.get_register(4) == 20);
	chip8.op_store(42, 4, 0);
	REQUIRE(chip8.get_register(4) == 42);
}

TEST_CASE("Op add 7XNN", "[chip8]")
{
	Chip8 chip8;

	// make sure this isn't changed
	auto carry = chip8.get_register(0xf);

	chip8.op_add(10, 7, 0);
	REQUIRE(chip8.get_register(7) == 10);
	REQUIRE(chip8.get_register(0xf) == carry);
	chip8.op_add(0, 7, 0);
	REQUIRE(chip8.get_register(7) == 10);
	REQUIRE(chip8.get_register(0xf) == carry);
	chip8.op_add(10, 7, 0);
	REQUIRE(chip8.get_register(7) == 20);
	REQUIRE(chip8.get_register(0xf) == carry);
	// wraps around, so should subtract 6
	chip8.op_add(250, 7, 0);
	REQUIRE(chip8.get_register(7) == 14);
	REQUIRE(chip8.get_register(0xf) == carry);
}

TEST_CASE("Op set 8XY0", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(60, 3, 0);
	chip8.op_set(0, 4, 3);
	REQUIRE(chip8.get_register(3) == 60);
	REQUIRE(chip8.get_register(4) == 60);
	chip8.op_set(0, 3, 2);
	REQUIRE(chip8.get_register(2) == 0);
	REQUIRE(chip8.get_register(3) == 0);
}

TEST_CASE("Op or 8XY1", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(0x0a, 2, 0);
	chip8.op_store(0xb0, 3, 0);

	chip8.op_or(0, 2, 3);
	REQUIRE(chip8.get_register(2) == 0xba);
	REQUIRE(chip8.get_register(3) == 0xb0);

	chip8.op_or(0, 2, 4);
	REQUIRE(chip8.get_register(2) == 0xba);
}

TEST_CASE("Op and 8XY2", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(0xbd, 2, 0);
	chip8.op_store(0xdb, 3, 0);

	chip8.op_and(0, 2, 3);
	REQUIRE(chip8.get_register(2) == 0x99);
	REQUIRE(chip8.get_register(3) == 0xdb);

	chip8.op_and(0, 2, 4);
	REQUIRE(chip8.get_register(2) == 0);
}

TEST_CASE("Op xor 8XY3", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(0xbd, 2, 0);
	chip8.op_store(0xdb, 3, 0);

	chip8.op_xor(0, 2, 3);
	REQUIRE(chip8.get_register(2) == 0x66);
	REQUIRE(chip8.get_register(3) == 0xdb);

	chip8.op_xor(0, 2, 2);
	REQUIRE(chip8.get_register(2) == 0);
}

TEST_CASE("Op madd 8XY4", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(10, 1, 0);
	chip8.op_store(45, 2, 0);

	chip8.op_madd(0, 1, 2);
	REQUIRE(chip8.get_register(1) == 55);
	REQUIRE(chip8.get_register(0xf) == 0);

	chip8.op_store(210, 3, 0);
	chip8.op_madd(0, 1, 3);
	REQUIRE(chip8.get_register(1) == 9);
	REQUIRE(chip8.get_register(0xf) == 1);

	chip8.op_madd(0, 1, 2);
	REQUIRE(chip8.get_register(1) == 54);
	REQUIRE(chip8.get_register(0xf) == 0);
}

TEST_CASE("Op sub 8XY5", "[chip8]")
{
	// TODO
}

TEST_CASE("Op shiftr 8XY6", "[chip8]")
{
	// TODO
}

TEST_CASE("Op rsub 8XY7", "[chip8]")
{
	// TODO
}

TEST_CASE("Op shiftl 8XYE", "[chip8]")
{
	// TODO
}

TEST_CASE("Op if_ncmp 9XY0", "[chip8]")
{
	// TODO
}

TEST_CASE("Op save ANNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Op jmp BNNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Op rand CXNN", "[chip8]")
{
	// TODO
}

TEST_CASE("Op disp DXYN", "[chip8]")
{
	// TODO
}

TEST_CASE("Op press EX9E", "[chip8]")
{
	// TODO
}

TEST_CASE("Op release EXA1", "[chip8]")
{
	// TODO
}

TEST_CASE("Op getdel FX07", "[chip8]")
{
	// TODO
}

TEST_CASE("Op wait FX0A", "[chip8]")
{
	// TODO
}

TEST_CASE("Op setdel FX15", "[chip8]")
{
	// TODO
}

TEST_CASE("Op setsnd FX18", "[chip8]")
{
	// TODO
}

TEST_CASE("Op inc FX1E", "[chip8]")
{
	// TODO
}

TEST_CASE("Op font FX29", "[chip8]")
{
	// TODO
}

TEST_CASE("Op deci FX33", "[chip8]")
{
	// TODO
}

TEST_CASE("Op dump FX55", "[chip8]")
{
	// TODO
}

TEST_CASE("Op load FX65", "[chip8]")
{
	// TODO
}
