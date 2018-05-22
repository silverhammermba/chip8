#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#define CHIP8_OP(NAME) void op_ ## NAME (uint16_t, uint8_t, uint8_t);

class Chip8
{
public:
	constexpr static unsigned int memory_size = 0x1000;
	constexpr static unsigned int registers_size = 0x10; // must be nibble-addressable
	constexpr static unsigned int stack_size = 48; // just initial size, can grow unbounded

	constexpr static uint16_t program_mem_start = 0x200;

	constexpr static unsigned int screen_width = 64;
	constexpr static unsigned int screen_height = 32;

	typedef void (Chip8::*opfn_t)(uint16_t, uint8_t, uint8_t);

private:
	std::array<uint8_t, memory_size> memory {}; // RAM
	std::array<uint8_t, registers_size> data_registers {}; // V0-VF
	uint16_t address_register = 0; // I

	std::vector<uint16_t> stack; // historically 24 frames, but how about infinite?
	uint16_t program_counter = program_mem_start;

	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;

	// I/O
	std::array<bool, screen_width * screen_height> screen {};
	std::array<bool, registers_size> keys {};

	bool waiting_for_input = false;
	uint8_t input_register = 0;

	bool screen_dirty = true;

	// randomness
	std::default_random_engine random_generator;
	std::uniform_int_distribution<uint8_t> uniform_distribution;

	uint8_t rng();

	bool set_pixel(uint8_t, uint8_t, bool);

	void reset();
public:
	// setup
	Chip8();
	void load_rom(const std::string&);
	template<size_t SIZE>
	void load_bytes(const std::array<uint8_t, SIZE>& bytes)
	{
		reset();
		for (uint16_t i = 0, m = program_mem_start; i < bytes.size() && m < memory_size; ++i, ++m)
		{
			memory[m] = bytes[i];
		}
	}

	// read state
	uint16_t get_program_counter() const;
	uint16_t get_address_register() const;
	uint8_t get_register(uint16_t) const;
	uint8_t get_memory(uint16_t) const;
	bool get_pixel(uint8_t, uint8_t) const;
	bool beep() const;

	// I/O
	void press(uint8_t);
	void release(uint8_t);

	bool should_draw();

	// emulate
	void step();

	// get an opcode from a position in memory
	static uint16_t get_opcode(std::array<uint8_t, memory_size>&, uint16_t);
	// turn an opcode into a method pointer and arguments for that method
	static std::tuple<opfn_t, uint16_t, uint8_t, uint8_t> decode_opcode(uint16_t);

	/* opcode implementations, all are prefixed with op_ to indicate that.
	 * The names don't need to be readable because normally these are called
	 * only by the interpreter.
	 */
	// intentionally ommitted 0NNN for calling RCA 1802 programs
	CHIP8_OP(clear); // 00E0
	CHIP8_OP(ret); // 00EE
	CHIP8_OP(goto); // 1NNN
	CHIP8_OP(call); // 2NNN
	CHIP8_OP(if_eq); // 3XNN
	CHIP8_OP(if_ne); // 4XNN
	CHIP8_OP(if_cmp); // 5XY0
	CHIP8_OP(store); //6XNN
	CHIP8_OP(add); // 7XNN
	CHIP8_OP(set); // 8XY0
	CHIP8_OP(or); // 8XY1
	CHIP8_OP(and); // 8XY2
	CHIP8_OP(xor); // 8XY3
	CHIP8_OP(madd); // 8XY4
	CHIP8_OP(sub); // 8XY5
	CHIP8_OP(shiftr); // 8XY6
	CHIP8_OP(rsub); // 8XY7
	CHIP8_OP(shiftl); // 8XYE
	CHIP8_OP(if_ncmp); // 9XY0
	CHIP8_OP(save); // ANNN
	CHIP8_OP(jmp); // BNNN
	CHIP8_OP(rand); // CXNN
	CHIP8_OP(disp); // DXYN
	CHIP8_OP(press); // EX9E
	CHIP8_OP(release); // EXA1
	CHIP8_OP(getdel); // FX07
	CHIP8_OP(wait); // FX0A
	CHIP8_OP(setdel); // FX15
	CHIP8_OP(setsnd); // FX18
	CHIP8_OP(inc); // FX1E
	CHIP8_OP(font); // FX29
	CHIP8_OP(deci); // FX33
	CHIP8_OP(dump); // FX55
	CHIP8_OP(load); // FX65
};

#undef CHIP8_OP
