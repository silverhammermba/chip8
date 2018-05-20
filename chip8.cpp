#include "chip8.hpp"

Chip8::Chip8()
{
	stack.reserve(stack_size);
}

void Chip8::step()
{
	uint16_t opcode = get_opcode(memory, program_counter);
	program_counter += 2;
}

uint16_t Chip8::get_opcode(std::array<uint8_t, memory_size>& _memory, uint16_t _counter)
{
	return (_memory[_counter] << 8) | _memory[_counter + 1];
}
