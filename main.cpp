#include <cstdlib>
#include <iostream>
#include <string>
#include "chip8.hpp"

void draw_screen(const Chip8& emu)
{
	for (uint8_t y = 0; y < Chip8::screen_height; ++y)
	{
		for (uint8_t x = 0; x < Chip8::screen_width; ++x)
		{
			std::cout << (emu.get_pixel(x, y) ? '#' : ' ');
		}
		std::cout << std::endl;
	}
}

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		std::cerr << "usage: " << argv[0] << " ROM\n";
		return EXIT_FAILURE;
	}

	Chip8 chip8;

	chip8.load_rom(argv[1]);

	std::string ignored;

	while (true)
	{
		chip8.step();
		if (chip8.should_draw())
		{
			draw_screen(chip8);
			std::getline(std::cin, ignored);
		}
	}

	return EXIT_SUCCESS;
}
