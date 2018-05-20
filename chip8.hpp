#include <array>
#include <cstdint>
#include <tuple>
#include <vector>

class Chip8
{
public:
	constexpr static unsigned int memory_size = 4096;
	constexpr static unsigned int registers_size = 16;
	constexpr static unsigned int stack_size = 48;
	typedef void (Chip8::*opfn_t)(uint16_t);

private:
	std::array<uint8_t, memory_size> memory; // RAM
	std::array<uint8_t, registers_size> data_registers; // V0-VF
	uint16_t address_register; // I

	// technically should be fixed size, but eh
	std::vector<uint16_t> stack;
	uint16_t program_counter;

	uint8_t delay_timer;
	uint8_t sound_timer;

public:
	Chip8();

	void step();

	static uint16_t get_opcode(std::array<uint8_t, memory_size>&, uint16_t);
	static std::tuple<opfn_t, uint16_t> decode_opcode(uint16_t);

	// opcode implementations
	void clear_screen(uint16_t);
};