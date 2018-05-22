#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.hpp"

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		std::cerr << "usage: " << argv[0] << " ROM\n";
		return EXIT_FAILURE;
	}

	Chip8 chip8;
	chip8.load_rom(argv[1]);

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow("CHIP8", 100, 100, Chip8::screen_width, Chip8::screen_height, SDL_WINDOW_SHOWN);

	if (!window)
	{
		std::cerr << "SDL_CreateWindow: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		std::cerr << "SDL_CreateRenderer: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
