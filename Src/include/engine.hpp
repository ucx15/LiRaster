#pragma once

#include "SDL.h"

#include "vec.hpp"
#include "color.hpp"
#include "surface.hpp"
#include "rect.hpp"
#include "tris.hpp"


class Engine {

	private:
		SDL_Window* sdl_window = NULL;
		SDL_Renderer* sdl_renderer = NULL;
		SDL_Surface* sdl_surface = NULL;
		SDL_Texture* sdl_texture = NULL;
		SDL_Event event;

		Surface _surface;
		Color* _buffer;  // Array of pixels

		bool isRunning;

	public:
		// Constructors and Destructors
		Engine();
		~Engine();

		// Methods
		void setup();
		void quit();

		void render();
		
		void handleEvents();
		void rasterize();

		int pipeline();
};
