#include <iostream>
#include <SDL.h>

#include "Chip8.h"

int main(int, char**) 
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,  window_flags);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC);

	const int tw = 64;
	const int th = 32;
	const int color_size = tw * th;
	
	//chip init
	int numKeys;
	const char* keys = (char*)SDL_GetKeyboardState(&numKeys);

	std::string path = "res/Pong.ch8";

	Chip8 emu;
	emu.setKeyMap(keys);
	emu.loadRom(path);

	Uint32 format = SDL_PIXELFORMAT_RGB332;
	SDL_Texture* texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, Chip8::buffer_width, Chip8::buffer_height);

	bool shouldQuit = false;
	while (!shouldQuit)
	{
		SDL_Event m_event;
		while (SDL_PollEvent(&m_event))
		{
			if (m_event.window.event == SDL_WINDOWEVENT_CLOSE)
				shouldQuit = 1;

			if (m_event.key.type == SDL_KEYUP)
			{
				for (char i = 0; i < 16; ++i)
				{
					char kname = i < 0xA ? '0' + i : 'A' + i - 10;

					char test[2] = { kname, '\0' };

					if (m_event.key.keysym.scancode == SDL_GetScancodeFromName(test))
						emu.heldkeys[i] = true;
				}
			}
		}
		emu.update();

		for (char i = 0; i < 16; ++i)
		{
			emu.heldkeys[i] = false;
		}

		if (emu.shouldDraw)
		{
			SDL_UpdateTexture(texture, NULL, emu.frame_buffer, Chip8::buffer_width * SDL_BYTESPERPIXEL(format));

			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	
	SDL_Quit();
	return 0;
}