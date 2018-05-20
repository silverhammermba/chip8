#include "chip8.hpp"

Chip8::Chip8()
{
	stack.reserve(stack_size);

	program_counter = 0x200;

	delay_timer = 0;
	sound_timer = 0;
}

void Chip8::step()
{
	uint16_t opcode = get_opcode(memory, program_counter);
	program_counter += 2; // each opcode is 2 bytes

	auto [op, n, x, y] = decode_opcode(opcode);
	(this->*op)(n, x, y);

	if (delay_timer > 0) --delay_timer;
	if (sound_timer > 0) --sound_timer;
}

uint16_t Chip8::get_opcode(std::array<uint8_t, memory_size>& _memory, uint16_t _counter)
{
	return (_memory[_counter] << 8) | _memory[_counter + 1];
}

std::tuple<Chip8::opfn_t, uint16_t, uint8_t, uint8_t> Chip8::decode_opcode(uint16_t)
{
	// TODO actually decode
	return {&Chip8::op_clear, 0, 0, 0};
}

#define CHIP8_OP(NAME) void Chip8::op_ ## NAME (uint16_t n, uint8_t x, uint8_t y)

// opcode implementations

CHIP8_OP(call_prg)
{
	// unimplemented, should call a RCA 1802 program at the given address
}

CHIP8_OP(clear)
{
	// TODO
}

CHIP8_OP(ret)
{
	program_counter = stack.back();
	stack.pop_back();
}

/*
CHIP8_OP(goto)
CHIP8_OP(call_sub)
CHIP8_OP(if_eq)
CHIP8_OP(if_ne)
CHIP8_OP(if_cmp)
CHIP8_OP(store)
CHIP8_OP(add)
CHIP8_OP(set)
CHIP8_OP(or)
CHIP8_OP(and)
CHIP8_OP(xor)
CHIP8_OP(madd)
CHIP8_OP(sub)
CHIP8_OP(shiftr)
CHIP8_OP(rsub)
CHIP8_OP(shiftl)
CHIP8_OP(if_ncmp)
CHIP8_OP(save)
CHIP8_OP(jmp)
CHIP8_OP(rand)
CHIP8_OP(disp)
CHIP8_OP(press)
CHIP8_OP(release)
CHIP8_OP(getdel)
CHIP8_OP(wait)
CHIP8_OP(setdel)
CHIP8_OP(setsnd)
CHIP8_OP(inc)
CHIP8_OP(font)
CHIP8_OP(deci)
CHIP8_OP(dump)
CHIP8_OP(load)
*/
