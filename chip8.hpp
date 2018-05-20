#include <array>


class Chip8
{
	constexpr static unsigned int memory_size = 4096;
	constexpr static unsigned int registers_size = 16;

	std::array<uint8_t, memory_size> memory;
	std::array<uint8_t, registers_size> registers;
public:
	Chip8();
};

