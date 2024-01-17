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
		float deltaTime;

		Surface surface;
		Color* buffer;  // Array of pixels

		Vec3* points;
		Vec3* ss_points;
		int* tris;

		int n_points;
		int n_tris;

		float ProjectionMatrix[4][4] = {0.f};

	public:
		Engine();
		~Engine();
		void pipeline();

	private:
		void setup();
		void quit();
		void handleEvents();

		void renderSetup();
		void project();
		void render();
		void rasterize();

};
