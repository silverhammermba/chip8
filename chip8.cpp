#include "chip8.hpp"

Chip8::Chip8()
{
	stack.reserve(stack_size);
}

void Chip8::step()
{
	uint16_t opcode = get_opcode(memory, program_counter);
	program_counter += 2;

	auto [op, arg] = decode_opcode(opcode);
	(this->*op)(arg);

	if (delay_timer > 0) --delay_timer;
	if (sound_timer > 0) --sound_timer;
}

uint16_t Chip8::get_opcode(std::array<uint8_t, memory_size>& _memory, uint16_t _counter)
{
	return (_memory[_counter] << 8) | _memory[_counter + 1];
}

std::tuple<Chip8::opfn_t, uint16_t> Chip8::decode_opcode(uint16_t)
{
	// TODO actually decode
	return {&Chip8::clear_screen, 0};
}


// opcode implementations

void Chip8::clear_screen(uint16_t unused)
{
	// TODO
}
