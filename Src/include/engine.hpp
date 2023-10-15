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

		bool isRunning;
		uint64_t frame_count;

		Surface surface;
		Color* buffer;  // Array of pixels

		Vec3* points;
		Vec3* ss_points;
		int* tris;


	public:
		Engine();
		~Engine();
		int pipeline();

	private:
		void setup();
		void quit();
		void handleEvents();

		void project();
		void render();
		void rasterize();

};
