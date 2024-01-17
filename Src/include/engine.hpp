#pragma once

#include "SDL.h"

#include "vec.hpp"
#include "color.hpp"
#include "surface.hpp"
#include "rect.hpp"
#include "tris.hpp"


class Engine {

	private:
		// SDL Stuff
		SDL_Window *SDLWindow     = NULL;
		SDL_Renderer *SDLRenderer = NULL;
		SDL_Surface *SDLSurface   = NULL;
		SDL_Texture *SDLTexture   = NULL;
		SDL_Event SDLEvent;

		// Engine Stuff
		Surface enSurface;
		Color *enBuffer;  // Array of pixels

		int nPoints;
		int nTris;

		Vec3 *points;
		Vec3 *ssPoints;
		int *tris;

		// Rendering Stuff
		bool isRunning;
		float deltaTime;

		glm::mat4 projMat;

	public:
		Engine();
		~Engine();
		void pipeline();

	private:
		void SDLSetup();
		void quit();
		void handleEvents();

		void engineSetup();
		void project();
		void render();
		void rasterize();
};
