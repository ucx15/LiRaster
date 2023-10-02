#pragma once


#include "vec.hpp"
#include "color.hpp"
#include "surface.hpp"
#include "rect.hpp"
#include "tris.hpp"


class Engine {

	private:
		Surface _surface;
		Color* _buffer;  // Array of pixels


	public:
		// Constructors and Destructors
		Engine();
		~Engine();

		// Methods
		void setup();
		void render();
		int pipeline();
		void quit();
};
