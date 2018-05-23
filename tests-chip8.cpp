#include <array>
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
	Chip8 chip8;

	chip8.op_font(0, 0, 0);
	chip8.op_disp(0, 0, 0);

	chip8.should_draw();

	chip8.op_clear(0, 0, 0);
	for (uint8_t x = 0; x < Chip8::screen_width; ++x)
	{
		for (uint8_t y = 0; y < Chip8::screen_height; ++y)
		{
			REQUIRE(chip8.get_pixel(x, y) == false);
		}
	}
	REQUIRE(chip8.should_draw() == true);
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

	SECTION("carry bit")
	{
		chip8.op_store(255, 10, 0);
		chip8.op_store(1, 12, 0);

		chip8.op_madd(0, 11, 10);
		REQUIRE(chip8.get_register(0xf) == 0);
		chip8.op_madd(0, 11, 12);
		REQUIRE(chip8.get_register(0xf) == 1);
	}

	SECTION("general addition")
	{
		chip8.op_store(10, 1, 0);
		chip8.op_store(45, 2, 0);

		chip8.op_madd(0, 1, 2);
		REQUIRE(chip8.get_register(1) == 55);
		REQUIRE(chip8.get_register(2) == 45);
		REQUIRE(chip8.get_register(0xf) == 0);

		chip8.op_store(210, 3, 0);
		chip8.op_madd(0, 1, 3);
		REQUIRE(chip8.get_register(1) == 9);
		REQUIRE(chip8.get_register(3) == 210);
		REQUIRE(chip8.get_register(0xf) == 1);

		chip8.op_madd(0, 1, 2);
		REQUIRE(chip8.get_register(1) == 54);
		REQUIRE(chip8.get_register(0xf) == 0);
	}
}

TEST_CASE("Op sub 8XY5", "[chip8]")
{
	Chip8 chip8;

	SECTION("check carry bit")
	{
		chip8.op_store(1, 10, 0);
		chip8.op_store(1, 12, 0);

		chip8.op_sub(0, 10, 12);
		REQUIRE(chip8.get_register(0xf) == 0);
		chip8.op_sub(0, 10, 12);
		REQUIRE(chip8.get_register(0xf) == 1);
	}

	SECTION("general subtraction")
	{
		chip8.op_store(10, 1, 0);
		chip8.op_store(45, 2, 0);

		chip8.op_sub(0, 2, 1);
		REQUIRE(chip8.get_register(2) == 35);
		REQUIRE(chip8.get_register(0xf) == 0);

		chip8.op_store(210, 3, 0);
		chip8.op_sub(0, 2, 3);
		REQUIRE(chip8.get_register(2) == 81);
		REQUIRE(chip8.get_register(3) == 210);
		REQUIRE(chip8.get_register(0xf) == 1);

		chip8.op_sub(0, 2, 1);
		REQUIRE(chip8.get_register(2) == 71);
		REQUIRE(chip8.get_register(0xf) == 0);
	}
}

TEST_CASE("Op shiftr 8XY6", "[chip8]")
{
	Chip8 chip8;

	chip8.op_shiftr(0, 2, 1);
	REQUIRE(chip8.get_register(2) == 0);
	REQUIRE(chip8.get_register(1) == 0);
	REQUIRE(chip8.get_register(0xf) == 0);

	chip8.op_store(0x1f, 1, 0);
	chip8.op_shiftr(0, 2, 1);
	REQUIRE(chip8.get_register(2) == 0xf);
	REQUIRE(chip8.get_register(1) == 0x1f);
	REQUIRE(chip8.get_register(0xf) == 1);
}

TEST_CASE("Op rsub 8XY7", "[chip8]")
{
	Chip8 chip8;

	SECTION("check carry bit")
	{
		chip8.op_store(1, 10, 0);
		chip8.op_store(1, 12, 0);

		chip8.op_rsub(0, 12, 10);
		REQUIRE(chip8.get_register(0xf) == 0);
		chip8.op_rsub(0, 10, 12);
		REQUIRE(chip8.get_register(0xf) == 1);
	}

	SECTION("general subtraction")
	{
		chip8.op_store(10, 1, 0);
		chip8.op_store(45, 2, 0);

		chip8.op_rsub(0, 1, 2);
		REQUIRE(chip8.get_register(1) == 35);
		REQUIRE(chip8.get_register(2) == 45);
		REQUIRE(chip8.get_register(0xf) == 0);

		chip8.op_store(210, 3, 0);
		chip8.op_rsub(0, 3, 2);
		REQUIRE(chip8.get_register(3) == 91);
		REQUIRE(chip8.get_register(2) == 45);
		REQUIRE(chip8.get_register(0xf) == 1);

		chip8.op_rsub(0, 2, 3);
		REQUIRE(chip8.get_register(2) == 46);
		REQUIRE(chip8.get_register(0xf) == 0);
	}
}

TEST_CASE("Op shiftl 8XYE", "[chip8]")
{
	Chip8 chip8;

	chip8.op_shiftl(0, 2, 1);
	REQUIRE(chip8.get_register(2) == 0);
	REQUIRE(chip8.get_register(1) == 0);
	REQUIRE(chip8.get_register(0xf) == 0);

	chip8.op_store(0x88, 1, 0);
	chip8.op_shiftl(0, 2, 1);
	REQUIRE(chip8.get_register(2) == 0x10);
	REQUIRE(chip8.get_register(1) == 0x10);
	REQUIRE(chip8.get_register(0xf) == 1);
}

TEST_CASE("Op if_ncmp 9XY0", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(10, 1, 0);
	chip8.op_store(10, 2, 0);
	chip8.op_store(20, 3, 0);

	SECTION("when unequal")
	{
		chip8.op_if_ncmp(0, 1, 3);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 2);
		chip8.op_if_ncmp(0, 2, 4);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
	}
	SECTION("when equal")
	{
		chip8.op_if_ncmp(0, 1, 2);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);
	}
}

TEST_CASE("Op save ANNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_save(0xc0d, 0, 0);
	REQUIRE(chip8.get_address_register() == 0xc0d);
}

TEST_CASE("Op jmp BNNN", "[chip8]")
{
	Chip8 chip8;

	chip8.op_jmp(0, 0, 0);
	REQUIRE(chip8.get_program_counter() == 0);

	chip8.op_jmp(10, 0, 0);
	REQUIRE(chip8.get_program_counter() == 10);

	chip8.op_store(5, 0, 0);
	chip8.op_jmp(10, 0, 0);
	REQUIRE(chip8.get_program_counter() == 15);
}

TEST_CASE("Op rand CXNN", "[chip8]")
{
	Chip8 chip8;

	SECTION("output matches mask")
	{
		uint8_t mask = 0x9a;

		int failures = 0;
		for (int i = 0; i < 10000; ++i)
		{
			chip8.op_rand(mask, 4, 0);
			if (chip8.get_register(4) & ~mask) ++failures;
		}
		REQUIRE(failures == 0);
	}

	SECTION("outputs roughly uniform values")
	{
		std::array<unsigned int, 0x100> counts {};

		for (int i = 0; i < 10000; ++i)
		{
			chip8.op_rand(0xff, 5, 0);
			++counts[chip8.get_register(5)];
		}
		int failures = 0;
		for (auto count : counts)
		{
			if (count == 0) ++failures;
		}
		REQUIRE(failures == 0);
	}
}

TEST_CASE("Op disp DXYN", "[chip8]")
{
	Chip8 chip8;

	chip8.load_bytes(std::array<uint8_t, 1>{0xff});
	chip8.op_save(Chip8::program_mem_start, 0, 0);
	chip8.should_draw();

	SECTION("drawing nothing")
	{
		chip8.op_disp(0, 2, 5);
		for (uint8_t x = 0; x < Chip8::screen_width; ++x)
		{
			for (uint8_t y = 0; y < Chip8::screen_height; ++y)
			{
				REQUIRE(chip8.get_pixel(x, y) == false);
			}
		}
		REQUIRE(chip8.should_draw() == false);
	}

	SECTION("wraparound")
	{
		chip8.op_store(Chip8::screen_width - 2, 2, 0);
		chip8.op_store(12, 5, 0);

		chip8.op_disp(1, 2, 5);

		REQUIRE(chip8.should_draw() == true);
		REQUIRE(chip8.get_register(0xf) == 0);

		REQUIRE(chip8.get_pixel(Chip8::screen_width - 2, 12) == true);
		REQUIRE(chip8.get_pixel(Chip8::screen_width - 1, 12) == true);
		REQUIRE(chip8.get_pixel(0, 12) == true);
		REQUIRE(chip8.get_pixel(1, 12) == true);
		REQUIRE(chip8.get_pixel(2, 12) == true);
	}

	SECTION("drawing and overdrawing")
	{
		chip8.op_store(10, 2, 0);
		chip8.op_store(12, 5, 0);

		chip8.op_disp(1, 2, 5);

		REQUIRE(chip8.should_draw() == true);

		// ensure that that line of pixels is set
		for (uint8_t x = 0; x < Chip8::screen_width; ++x)
		{
			for (uint8_t y = 0; y < Chip8::screen_height; ++y)
			{
				REQUIRE(chip8.get_pixel(x, y) == (y == 12 && (x >= 10 && x < 18)));
			}
		}
		// and no carry
		REQUIRE(chip8.get_register(0xf) == 0);

		// still no carry for non-overlapping sprite
		chip8.op_add(1, 5, 0);
		chip8.op_disp(1, 2, 5);
		REQUIRE(chip8.get_register(0xf) == 0);

		// overlapping write should clear it and set carry
		chip8.op_add(255, 5, 0);
		chip8.op_disp(1, 2, 5);
		for (uint8_t x = 0; x < Chip8::screen_width; ++x)
		{
			REQUIRE(chip8.get_pixel(x, 12) == false);
		}
		REQUIRE(chip8.get_register(0xf) == 1);
	}
}

TEST_CASE("Op press EX9E", "[chip8]")
{
	Chip8 chip8;

	chip8.press(0xa);
	chip8.op_store(0xa, 1, 0);

	SECTION("when pressed")
	{
		chip8.op_press(0, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 2);
		chip8.op_press(0, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
		chip8.release(0xa);
		chip8.op_press(0, 1, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
	}
	SECTION("when not pressed")
	{
		chip8.op_press(0, 2, 0);
		REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);
	}
}

TEST_CASE("Op release EXA1", "[chip8]")
{
	Chip8 chip8;

	chip8.press(0xa);
	chip8.op_store(0xa, 1, 0);

	chip8.op_release(0, 2, 0);
	REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 2);
	chip8.op_release(0, 2, 0);
	REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
	chip8.press(0);
	chip8.op_release(0, 2, 0);
	REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start + 4);
}

TEST_CASE("Op getdel/setdel FX07/FX15", "[chip8]")
{
	Chip8 chip8;

	chip8.op_store(20, 8, 0);
	chip8.op_setdel(0, 8, 0);
	chip8.op_getdel(0, 9, 0);
	REQUIRE(chip8.get_register(9) == 20);

	// set current instruction to 0x1200 (a noop)
	chip8.op_save(chip8.get_program_counter(), 0, 0);
	chip8.op_store(0x12, 0, 0);
	chip8.op_store(0x00, 1, 0);
	chip8.op_dump(0, 1, 0);

	// step 19 times, should be at 1
	for (int i = 0; i < 19; ++i) chip8.step();
	chip8.op_getdel(0, 4, 0);
	REQUIRE(chip8.get_register(4) == 1);

	// then 0
	chip8.step();
	chip8.op_getdel(0, 4, 0);
	REQUIRE(chip8.get_register(4) == 0);

	// then stay at 0
	chip8.step();
	chip8.op_getdel(0, 4, 0);
	REQUIRE(chip8.get_register(4) == 0);
}

TEST_CASE("Op wait FX0A", "[chip8]")
{
	Chip8 chip8;

	// set current instruction to 0x1BED (goto 0xBED)
	chip8.op_save(chip8.get_program_counter(), 0, 0);
	chip8.op_store(0x1b, 0, 0);
	chip8.op_store(0xed, 1, 0);
	chip8.op_dump(0, 1, 0);

	chip8.op_wait(0, 4, 0);

	chip8.step(); // should do nothing
	REQUIRE(chip8.get_program_counter() == Chip8::program_mem_start);

	chip8.press(0xa);
	REQUIRE(chip8.get_register(4) == 0xa);
	chip8.step();
	REQUIRE(chip8.get_program_counter() == 0xbed);
}

TEST_CASE("Op setsnd FX18", "[chip8]")
{
	Chip8 chip8;

	REQUIRE(chip8.beep() == false);

	chip8.op_store(10, 9, 0);
	chip8.op_setsnd(0, 9, 0);
	REQUIRE(chip8.beep() == true);

	// set current instruction to 0x1200 (a noop)
	chip8.op_save(chip8.get_program_counter(), 0, 0);
	chip8.op_store(0x12, 0, 0);
	chip8.op_store(0x00, 1, 0);
	chip8.op_dump(0, 1, 0);

	// step 9 times, should be at last step
	for (int i = 0; i < 9; ++i) chip8.step();
	REQUIRE(chip8.beep() == true);

	// no more beep
	chip8.step();
	REQUIRE(chip8.beep() == false);

	// still no beep
	chip8.step();
	REQUIRE(chip8.beep() == false);
}

TEST_CASE("Op inc FX1E", "[chip8]")
{
	Chip8 chip8;

	SECTION("incrementing normally")
	{
		uint16_t start = chip8.get_address_register();

		chip8.op_store(3, 0xe, 0);
		chip8.op_inc(0, 0xe, 0);
		REQUIRE(chip8.get_address_register() == start + 3);
		REQUIRE(chip8.get_register(0xf) == 0);

		chip8.op_inc(0, 0xd, 0);
		REQUIRE(chip8.get_address_register() == start + 3);
		REQUIRE(chip8.get_register(0xf) == 0);
	}

	SECTION("incrementing with overflow")
	{
		chip8.op_save(Chip8::memory_size - 2, 0, 0);

		chip8.op_store(1, 0xd, 0);
		chip8.op_inc(0, 0xd, 0);

		REQUIRE(chip8.get_address_register() == Chip8::memory_size - 1);
		REQUIRE(chip8.get_register(0xf) == 0);

		chip8.op_store(3, 0xe, 0);
		chip8.op_inc(0, 0xe, 0);

		REQUIRE(chip8.get_address_register() == 2);
		REQUIRE(chip8.get_register(0xf) == 1);
	}
}

TEST_CASE("Op font FX29", "[chip8]")
{
	Chip8 chip8;

	chip8.op_font(0, 0, 0);
	REQUIRE(chip8.get_address_register() == 0x50);

	chip8.op_store(0xe, 1, 0);
	chip8.op_font(0, 1, 0);
	REQUIRE(chip8.get_address_register() == 0x50 + 0xe * 5);
}

TEST_CASE("Op deci FX33", "[chip8]")
{
	Chip8 chip8;
	chip8.op_save(0x980, 0, 0);

	SECTION("decimal-code 0")
	{
		chip8.op_deci(0, 0, 0);
		REQUIRE(chip8.get_memory(0x980) == 0);
		REQUIRE(chip8.get_memory(0x981) == 0);
		REQUIRE(chip8.get_memory(0x982) == 0);
	}

	SECTION("decimal-code 7")
	{
		chip8.op_store(7, 1, 0);
		chip8.op_deci(0, 1, 0);
		REQUIRE(chip8.get_memory(0x980) == 0);
		REQUIRE(chip8.get_memory(0x981) == 0);
		REQUIRE(chip8.get_memory(0x982) == 7);
	}

	SECTION("decimal-code 89")
	{
		chip8.op_store(89, 1, 0);
		chip8.op_deci(0, 1, 0);
		REQUIRE(chip8.get_memory(0x980) == 0);
		REQUIRE(chip8.get_memory(0x981) == 8);
		REQUIRE(chip8.get_memory(0x982) == 9);
	}

	SECTION("decimal-code 255")
	{
		chip8.op_store(255, 1, 0);
		chip8.op_deci(0, 1, 0);
		REQUIRE(chip8.get_memory(0x980) == 2);
		REQUIRE(chip8.get_memory(0x981) == 5);
		REQUIRE(chip8.get_memory(0x982) == 5);
	}
}

TEST_CASE("Op dump FX55", "[chip8]")
{
	Chip8 chip8;
	chip8.op_save(0xfad, 0, 0);

	SECTION("dump nothing")
	{
		chip8.op_dump(0, 0, 0);
		REQUIRE(chip8.get_memory(0xfad) == 0);
		REQUIRE(chip8.get_address_register() == 0xfae);
	}

	SECTION("dump some")
	{
		chip8.op_store(12, 0, 0);
		chip8.op_store(7, 1, 0);
		chip8.op_store(0xfe, 2, 0);
		chip8.op_store(0xa, 3, 0);
		chip8.op_store(2, 4, 0);
		chip8.op_store(3, 5, 0);

		chip8.op_dump(0, 5, 0);
		REQUIRE(chip8.get_memory(0xfad) == 12);
		REQUIRE(chip8.get_memory(0xfae) == 7);
		REQUIRE(chip8.get_memory(0xfaf) == 0xfe);
		REQUIRE(chip8.get_memory(0xfb0) == 0xa);
		REQUIRE(chip8.get_memory(0xfb1) == 2);
		REQUIRE(chip8.get_memory(0xfb2) == 3);
		REQUIRE(chip8.get_address_register() == 0xfb3);
	}

	SECTION("dump all")
	{
		for (uint8_t i = 0; i < Chip8::registers_size; ++i) chip8.op_store(i * 2, i, 0);

		chip8.op_dump(0, Chip8::registers_size - 1, 0);

		for (uint8_t i = 0; i < Chip8::registers_size; ++i) REQUIRE(chip8.get_memory(0xfad + i) == i * 2);
		REQUIRE(chip8.get_address_register() == 0xfad + Chip8::registers_size);
	}
}

TEST_CASE("Op load FX65", "[chip8]")
{
	Chip8 chip8;

	// dump 0,2,4,... starting at 0xbad
	chip8.op_save(0xbad, 0, 0);
	for (uint8_t i = 0; i < Chip8::registers_size; ++i) chip8.op_store(i * 2, i, 0);
	chip8.op_dump(0, Chip8::registers_size - 1, 0);
	// reset address to 0xbad and clear registers
	chip8.op_save(0xbad, 0, 0);
	for (uint8_t i = 0; i < Chip8::registers_size; ++i) chip8.op_store(0, i, 0);

	SECTION("load nothing")
	{
		chip8.op_load(0, 0, 0);
		REQUIRE(chip8.get_register(0) == 0);
		REQUIRE(chip8.get_address_register() == 0xbae);
	}

	SECTION("load all")
	{
		chip8.op_load(0, Chip8::registers_size - 1, 0);

		for (uint8_t i = 0; i < Chip8::registers_size; ++i) REQUIRE(chip8.get_register(i) == i * 2);
	}
}

TEST_CASE("Indicates when screen should redraw")
{
	Chip8 chip8;

	REQUIRE(chip8.should_draw() == true);
	REQUIRE(chip8.should_draw() == false);
}
