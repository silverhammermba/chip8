#include <cstdlib>
#include <iostream>
#include <unordered_map>
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

	bool running = true;

	std::unordered_map<SDL_Scancode, uint8_t> keymap = {
		{SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2}, {SDL_SCANCODE_3, 0x3}, {SDL_SCANCODE_4, 0xc},
		{SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5}, {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xd},
		{SDL_SCANCODE_A, 0x7}, {SDL_SCANCODE_S, 0x8}, {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xe},
		{SDL_SCANCODE_Z, 0xa}, {SDL_SCANCODE_X, 0x0}, {SDL_SCANCODE_C, 0xb}, {SDL_SCANCODE_V, 0xf},
	};

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE) running = false;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
					SDL_Scancode code = event.key.keysym.scancode;
					if (code == SDL_SCANCODE_ESCAPE)
					{
						running = false;
						break;
					}

					if (keymap.count(code))
					{
						if (event.type == SDL_KEYDOWN)
							chip8.press(keymap.at(code));
						else
							chip8.release(keymap.at(code));
					}
					break;
				}
			}
		}

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
			for (uint8_t x = 0; x < Chip8::screen_width; ++x)
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
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
