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

		int enVxCount;
		int enTriCount;

		Vec3 *enVerticies;
		Vec3 *enSSVerticies;
		int *enTriIndex;

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
		void loadScene();
		void project();
		void render();
		void rasterize();
};
