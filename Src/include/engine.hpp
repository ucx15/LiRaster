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
		SDL_Window *SDLWindow;
		SDL_Renderer *SDLRenderer;
		SDL_Texture *SDLTexture;
		SDL_Event SDLEvent;

		// Engine Stuff
		uint32_t *enTextureBuffer;  // intermediate Buffer for writing to SDLTexture
		Color *enBuffer;            // Array of pixels
		Surface enSurface;

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
