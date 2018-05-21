#include "chip8.hpp"

Chip8::Chip8()
{
	stack.reserve(stack_size);

	program_counter = program_mem_start;

	delay_timer = 0;
	sound_timer = 0;
}

uint16_t Chip8::get_program_counter() const
{
	return program_counter;
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

#define OP_PTR(NAME) (&Chip8::op_ ## NAME )

std::tuple<Chip8::opfn_t, uint16_t, uint8_t, uint8_t> Chip8::decode_opcode(uint16_t opcode)
{
	switch (opcode >> 12)
	{
		case 0x0:
			if (opcode == 0x00e0) return {OP_PTR(clear), 0, 0, 0};
			if (opcode == 0x00ee) return {OP_PTR(ret), 0, 0, 0};
			break;
		case 0x1:
			return {OP_PTR(goto), opcode & 0x0fff, 0, 0};
		case 0x2:
			return {OP_PTR(call), opcode & 0x0fff, 0, 0};
		case 0x3:
			return {OP_PTR(if_eq), opcode & 0x00ff, (opcode >> 8) & 0xf, 0};
		case 0x4:
			return {OP_PTR(if_ne), opcode & 0x00ff, (opcode >> 8) & 0xf, 0};
		case 0x5:
			if ((opcode & 0xf) != 0x0) break;
			return {OP_PTR(if_cmp), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
		case 0x6:
			return {OP_PTR(store), opcode & 0x00ff, (opcode >> 8) & 0xf, 0};
		case 0x7:
			return {OP_PTR(add), opcode & 0x00ff, (opcode >> 8) & 0xf, 0};
		case 0x8:
			switch (opcode & 0xf)
			{
				case 0x0:
					return {OP_PTR(set), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x1:
					return {OP_PTR(or), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x2:
					return {OP_PTR(and), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x3:
					return {OP_PTR(xor), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x4:
					return {OP_PTR(madd), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x5:
					return {OP_PTR(sub), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x6:
					return {OP_PTR(shiftr), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x7:
					return {OP_PTR(rsub), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0xe:
					return {OP_PTR(shiftl), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
			}
			break;
		case 0x9:
			if ((opcode & 0xf) != 0x0) break;
			return {OP_PTR(if_ncmp), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
		case 0xa:
			return {OP_PTR(save), opcode & 0x0fff, 0, 0};
		case 0xb:
			return {OP_PTR(jmp), opcode & 0x0fff, 0, 0};
		case 0xc:
			return {OP_PTR(rand), opcode & 0x00ff, (opcode >> 8) & 0xf, 0};
		case 0xd:
			return {OP_PTR(disp), opcode & 0x000f, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
		case 0xe:
			if ((opcode & 0xff) == 0x9e) return {OP_PTR(press), 0, (opcode >> 8) & 0xf, 0};
			if ((opcode & 0xff) == 0xa1) return {OP_PTR(release), 0, (opcode >> 8) & 0xf, 0};
			break;
		case 0xf:
			switch (opcode & 0xff)
			{
				case 0x07:
					return {OP_PTR(getdel), 0, (opcode >> 8) & 0xf, 0};
				case 0x0a:
					return {OP_PTR(wait), 0, (opcode >> 8) & 0xf, 0};
				case 0x15:
					return {OP_PTR(setdel), 0, (opcode >> 8) & 0xf, 0};
				case 0x18:
					return {OP_PTR(setsnd), 0, (opcode >> 8) & 0xf, 0};
				case 0x1e:
					return {OP_PTR(inc), 0, (opcode >> 8) & 0xf, 0};
				case 0x29:
					return {OP_PTR(font), 0, (opcode >> 8) & 0xf, 0};
				case 0x33:
					return {OP_PTR(deci), 0, (opcode >> 8) & 0xf, 0};
				case 0x55:
					return {OP_PTR(dump), 0, (opcode >> 8) & 0xf, 0};
				case 0x65:
					return {OP_PTR(load), 0, (opcode >> 8) & 0xf, 0};
			}
			break;
	}

	// TODO log a fault or something?
	return {nullptr, 0, 0, 0};
}

// macros for defining op implementations. since all ops accept all arguments, just omit names of unused ones
#define CHIP8_OP(NAME) void Chip8::op_ ## NAME (uint16_t, uint8_t, uint8_t)
#define CHIP8_OP_X(NAME) void Chip8::op_ ## NAME (uint16_t, uint8_t x, uint8_t)
#define CHIP8_OP_N(NAME) void Chip8::op_ ## NAME (uint16_t n, uint8_t, uint8_t)
#define CHIP8_OP_XY(NAME) void Chip8::op_ ## NAME (uint16_t, uint8_t x, uint8_t y)
#define CHIP8_OP_XN(NAME) void Chip8::op_ ## NAME (uint16_t n, uint8_t x, uint8_t)
#define CHIP8_OP_XYN(NAME) void Chip8::op_ ## NAME (uint16_t n, uint8_t x, uint8_t y)

// opcode implementations

CHIP8_OP(clear)
{
	// TODO
}

CHIP8_OP(ret)
{
	program_counter = stack.back();
	stack.pop_back();
}

CHIP8_OP_N(goto)
{
	program_counter = n;
}

CHIP8_OP_N(call)
{
	stack.push_back(program_counter);
	program_counter = n;
}

CHIP8_OP_XN(if_eq)
{
	if (data_registers[x] == n) program_counter += 2;
}

CHIP8_OP_XN(if_ne)
{
	if (data_registers[x] != n) program_counter += 2;
}

CHIP8_OP_XY(if_cmp)
{
	if (data_registers[x] == data_registers[y]) program_counter += 2;
}

CHIP8_OP_XN(store)
{
	data_registers[x] = n;
}

CHIP8_OP_XN(add)
{
	data_registers[x] += n;
}

CHIP8_OP_XY(set)
{
	data_registers[x] = data_registers[y];
}

CHIP8_OP_XY(or)
{
	data_registers[x] |= data_registers[y];
}

CHIP8_OP_XY(and)
{
	data_registers[x] &= data_registers[y];
}

CHIP8_OP_XY(xor)
{
	data_registers[x] ^= data_registers[y];
}

CHIP8_OP_XY(madd)
{
	bool carry = data_registers[y] >= (0x10 - data_registers[x]);
	data_registers[x] += data_registers[y];
	data_registers[0xf] = carry;
}

CHIP8_OP(sub)
{
}

CHIP8_OP(shiftr)
{
}

CHIP8_OP(rsub)
{
}

CHIP8_OP(shiftl)
{
}

CHIP8_OP(if_ncmp)
{
}

CHIP8_OP(save)
{
}

CHIP8_OP(jmp)
{
}

CHIP8_OP(rand)
{
}

CHIP8_OP(disp)
{
}

CHIP8_OP(press)
{
}

CHIP8_OP(release)
{
}

CHIP8_OP(getdel)
{
}

CHIP8_OP(wait)
{
}

CHIP8_OP(setdel)
{
}

CHIP8_OP(setsnd)
{
}

CHIP8_OP(inc)
{
}

CHIP8_OP(font)
{
}

CHIP8_OP(deci)
{
}

CHIP8_OP(dump)
{
}

CHIP8_OP(load)
{
}
