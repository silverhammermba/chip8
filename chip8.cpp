#include <fstream>
#include "chip8.hpp"

uint8_t Chip8::rng()
{
	return uniform_distribution(random_generator);
}

// XOR pixel x,y with set, return true if it was turned off
bool Chip8::set_pixel(uint8_t x, uint8_t y, bool set)
{
	x %= screen_width;
	y %= screen_height;

	bool prev = get_pixel(x, y);
	screen.at(x + y * screen_width) = prev ^ set;
	return prev && !get_pixel(x, y);
}

// TODO seed RNG somehow?
Chip8::Chip8() : random_generator(std::random_device()()), uniform_distribution(0, 0xff)
{
	reset();
}

void Chip8::reset()
{
	memory.fill(0);
	data_registers.fill(0);
	program_counter = address_register = program_mem_start;

	stack.clear();
	stack.reserve(stack_size);

	delay_timer = 0;
	sound_timer = 0;

	screen.fill(false);
	keys.fill(false);

	waiting_for_input = false;
	screen_dirty = true;

	// 0
	memory[0x50] = 0b01100000;
	memory[0x51] = 0b10010000;
	memory[0x52] = 0b10010000;
	memory[0x53] = 0b10010000;
	memory[0x54] = 0b01100000;
	// 1
	memory[0x55] = 0b00100000;
	memory[0x56] = 0b01100000;
	memory[0x57] = 0b00100000;
	memory[0x58] = 0b00100000;
	memory[0x59] = 0b01110000;
	// 2
	memory[0x5a] = 0b11100000;
	memory[0x5b] = 0b00010000;
	memory[0x5c] = 0b01100000;
	memory[0x5d] = 0b10000000;
	memory[0x5e] = 0b11110000;
	// 3
	memory[0x5f] = 0b11100000;
	memory[0x60] = 0b00010000;
	memory[0x61] = 0b01100000;
	memory[0x62] = 0b00010000;
	memory[0x63] = 0b11100000;
	// 4
	memory[0x64] = 0b10100000;
	memory[0x65] = 0b10100000;
	memory[0x66] = 0b11110000;
	memory[0x67] = 0b00100000;
	memory[0x68] = 0b00100000;
	// 5
	memory[0x69] = 0b11110000;
	memory[0x6a] = 0b10000000;
	memory[0x6b] = 0b11100000;
	memory[0x6c] = 0b00010000;
	memory[0x6d] = 0b11100000;
	// 6
	memory[0x6e] = 0b01110000;
	memory[0x6f] = 0b10000000;
	memory[0x70] = 0b11100000;
	memory[0x71] = 0b10010000;
	memory[0x72] = 0b11100000;
	// 7
	memory[0x73] = 0b11110000;
	memory[0x74] = 0b00010000;
	memory[0x75] = 0b00100000;
	memory[0x76] = 0b01000000;
	memory[0x77] = 0b01000000;
	// 8
	memory[0x78] = 0b01100000;
	memory[0x79] = 0b10010000;
	memory[0x7a] = 0b01100000;
	memory[0x7b] = 0b10010000;
	memory[0x7c] = 0b01100000;
	// 9
	memory[0x7d] = 0b01100000;
	memory[0x7e] = 0b10010000;
	memory[0x7f] = 0b01110000;
	memory[0x80] = 0b00010000;
	memory[0x81] = 0b11100000;
	// A
	memory[0x82] = 0b01100000;
	memory[0x83] = 0b10010000;
	memory[0x84] = 0b11110000;
	memory[0x85] = 0b10010000;
	memory[0x86] = 0b10010000;
	// B
	memory[0x87] = 0b11100000;
	memory[0x88] = 0b10010000;
	memory[0x89] = 0b11100000;
	memory[0x8a] = 0b10010000;
	memory[0x8b] = 0b11100000;
	// C
	memory[0x8c] = 0b01110000;
	memory[0x8d] = 0b10000000;
	memory[0x8e] = 0b10000000;
	memory[0x8f] = 0b10000000;
	memory[0x90] = 0b01110000;
	// D
	memory[0x91] = 0b11100000;
	memory[0x92] = 0b10010000;
	memory[0x93] = 0b10010000;
	memory[0x94] = 0b10010000;
	memory[0x95] = 0b11100000;
	// E
	memory[0x96] = 0b11110000;
	memory[0x97] = 0b10000000;
	memory[0x98] = 0b11100000;
	memory[0x99] = 0b10000000;
	memory[0x9a] = 0b11110000;
	// F
	memory[0x9b] = 0b11110000;
	memory[0x9c] = 0b10000000;
	memory[0x9d] = 0b11100000;
	memory[0x9e] = 0b10000000;
	memory[0x9f] = 0b10000000;
}

void Chip8::load_rom(const std::string& filename)
{
	reset();
	std::ifstream romfile(filename);
	romfile.read(reinterpret_cast<char*>(memory.data()) + program_mem_start, memory_size - program_mem_start);
}

uint16_t Chip8::get_program_counter() const
{
	return program_counter;
}

uint16_t Chip8::get_address_register() const
{
	return address_register;
}

uint8_t Chip8::get_register(uint16_t x) const
{
	return data_registers.at(x);
}

uint8_t Chip8::get_memory(uint16_t n) const
{
	return memory.at(n);
}

bool Chip8::get_pixel(uint8_t x, uint8_t y) const
{
	return screen.at(x + y * screen_width);
}

bool Chip8::beep() const
{
	// TODO does this beep continuously when it's positive, or only once when it hits 0?
	return sound_timer > 0;
}

void Chip8::press(uint8_t key)
{
	if (!keys.at(key) && waiting_for_input)
	{
		waiting_for_input = false;
		data_registers.at(input_register) = key;
	}
	keys.at(key) = true;
}

void Chip8::release(uint8_t key)
{
	keys.at(key) = false;
}

bool Chip8::should_draw()
{
	bool tmp = screen_dirty;
	screen_dirty = false;
	return tmp;
}

void Chip8::step()
{
	if (waiting_for_input) return;

	if (delay_timer > 0) --delay_timer;
	if (sound_timer > 0) --sound_timer;

	uint16_t opcode = get_opcode(memory, program_counter);
	program_counter += 2; // each opcode is 2 bytes

	auto [op, n, x, y] = decode_opcode(opcode);
	(this->*op)(n, x, y);
}

uint16_t Chip8::get_opcode(std::array<uint8_t, memory_size>& _memory, uint16_t _counter)
{
	return (_memory.at(_counter) << 8) | _memory.at(_counter + 1);
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
					// Y passed, but unused
					return {OP_PTR(shiftr), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0x7:
					return {OP_PTR(rsub), 0, (opcode >> 8) & 0xf, (opcode >> 4) & 0xf};
				case 0xe:
					// Y passed, but unused
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
	screen.fill(0);
	screen_dirty = true;
}

CHIP8_OP(ret)
{
	// TODO log a fault or something?
	if (stack.empty()) return;

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
	if (data_registers.at(x) == n) program_counter += 2;
}

CHIP8_OP_XN(if_ne)
{
	if (data_registers.at(x) != n) program_counter += 2;
}

CHIP8_OP_XY(if_cmp)
{
	if (data_registers.at(x) == data_registers.at(y)) program_counter += 2;
}

CHIP8_OP_XN(store)
{
	data_registers.at(x) = n;
}

CHIP8_OP_XN(add)
{
	data_registers.at(x) += n;
}

CHIP8_OP_XY(set)
{
	data_registers.at(x) = data_registers.at(y);
}

CHIP8_OP_XY(or)
{
	data_registers.at(x) |= data_registers.at(y);
}

CHIP8_OP_XY(and)
{
	data_registers.at(x) &= data_registers.at(y);
}

CHIP8_OP_XY(xor)
{
	data_registers.at(x) ^= data_registers.at(y);
}

CHIP8_OP_XY(madd)
{
	bool carry = data_registers.at(y) >= (0x100 - data_registers.at(x));
	data_registers.at(x) += data_registers.at(y);
	data_registers[0xf] = carry;
}

CHIP8_OP_XY(sub)
{
	bool carry = data_registers.at(y) > data_registers.at(x);
	data_registers.at(x) -= data_registers.at(y);
	data_registers[0xf] = carry;
}

CHIP8_OP_X(shiftr)
{
	data_registers[0xf] = data_registers.at(x) & 1;
	data_registers.at(x) >>= 1;
}

CHIP8_OP_XY(rsub)
{
	bool carry = data_registers.at(y) >= data_registers.at(x);
	data_registers.at(x) = data_registers.at(y) - data_registers.at(x);
	data_registers[0xf] = carry;
}

CHIP8_OP_X(shiftl)
{
	data_registers[0xf] = (data_registers.at(x) & 0x80) != 0;
	data_registers.at(x) <<= 1;
}

CHIP8_OP_XY(if_ncmp)
{
	if (data_registers.at(x) != data_registers.at(y)) program_counter += 2;
}

CHIP8_OP_N(save)
{
	address_register = n;
}

CHIP8_OP_N(jmp)
{
	program_counter = data_registers.at(0) + n;
}

CHIP8_OP_XN(rand)
{
	data_registers.at(x) = rng() & n;
}

CHIP8_OP_XYN(disp)
{
	// address register can be over 0x1000???
	uint16_t sprite_address = address_register;
	data_registers[0xf] = 0;

	for (uint8_t line = 0; line < n; ++line)
	{
		uint8_t row = data_registers.at(y) + line;
		uint8_t sprite_data = memory.at(sprite_address++);

		for (uint8_t bit = 8; bit --> 0;)
		{
			data_registers[0xf] |= set_pixel(data_registers.at(x) + bit, row, sprite_data & 1);
			sprite_data >>= 1;
		}
	}
	screen_dirty = n > 0;
}

CHIP8_OP_X(press)
{
	if (keys.at(data_registers.at(x))) program_counter += 2;
}

CHIP8_OP_X(release)
{
	if (!keys.at(data_registers.at(x))) program_counter += 2;
}

CHIP8_OP_X(getdel)
{
	data_registers.at(x) = delay_timer;
}

CHIP8_OP_X(wait)
{
	waiting_for_input = true;
	input_register = x;
}

CHIP8_OP_X(setdel)
{
	delay_timer = data_registers.at(x);
}

CHIP8_OP_X(setsnd)
{
	sound_timer = data_registers.at(x);
}

CHIP8_OP_X(inc)
{
	data_registers[0xf] = address_register >= memory_size - data_registers.at(x);
	address_register = (address_register + data_registers.at(x)) % memory_size;
}

CHIP8_OP_X(font)
{
	// TODO can only find this documented for x=0x0-0xf. what about others?
	address_register = 0x50 + data_registers.at(x) * 5;
}

CHIP8_OP_X(deci)
{
	uint8_t num = data_registers.at(x);

	memory.at(address_register + 0) = num / 100;
	memory.at(address_register + 1) = (num % 100) / 10;
	memory.at(address_register + 2) = num % 10;
}

CHIP8_OP_X(dump)
{
	for (uint8_t i = 0; i <= x; ++i) memory.at(address_register++) = data_registers.at(i);
}

CHIP8_OP_X(load)
{
	for (uint8_t i = 0; i <= x; ++i) data_registers.at(i) = memory.at(address_register++);
}
