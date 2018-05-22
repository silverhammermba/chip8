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

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	unsigned int scale = 10;

	SDL_Window* window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Chip8::screen_width * scale, Chip8::screen_height * scale, 0);

	if (!window)
	{
		std::cerr << "SDL_CreateWindow: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		std::cerr << "SDL_CreateRenderer: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// draw for 5 seconds
	int frames = 60 * 5;

	while (true)
	{
		chip8.step();

		if (chip8.should_draw())
		{
			// clear black
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			// prepare pixels for drawing
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_Rect pixel;
			pixel.w = scale;
			pixel.h = scale;

			// draw each pixel
			for (uint8_t x = 0; x < Chip8::screen_height; ++x)
			{
				for (uint8_t y = 0; y < Chip8::screen_height; ++y)
				{
					if (chip8.get_pixel(x, y))
					{
						pixel.x = x * scale;
						pixel.y = y * scale;
						SDL_RenderFillRect(renderer, &pixel);
					}
				}
			}

			SDL_RenderPresent(renderer);
		}

		SDL_Delay(16);

		if (--frames < 0) break;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
