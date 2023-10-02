#include <iostream>
#include <chrono>
#include <math.h>

#include "SDL.h"
#include "engine.hpp"
#include "settings.hpp"
#include "utils.hpp"

using namespace std::chrono;


#define TIME_PT high_resolution_clock::now()
#define TIME_CAST_US(b, a) duration_cast<microseconds>(b - a)


// Cosntructors and Destructors
Engine::Engine() {
	this->setup();
}

Engine::~Engine() {
	this->quit();
}

void Engine::setup() {
    sdl_window = SDL_CreateWindow("LiRaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
	if (sdl_window == NULL) {
		SDL_Log("SDL_CreateWindow creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

    sdl_renderer =  SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
	if (sdl_renderer == NULL) {
		SDL_Log("SDL_CreateRenderer creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	sdl_surface = SDL_CreateRGBSurface(0, W, H, 32,0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF );
	if (sdl_surface == NULL) {
		SDL_Log("SDL_CreateRGBSurface failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if(SDL_Init(SDL_INIT_EVERYTHING)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}


	_buffer = new Color[W*H];

	_surface = Surface(_buffer, W, H);
	isRunning = true;
}


void Engine::quit() {
	// Cleanup
	delete[] _buffer;

	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();

}


void Engine::handleEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}
	}
}


void Engine::rasterize() {
	SDL_LockSurface(sdl_surface);
		_surface.toU32Surface((uint32_t*)sdl_surface->pixels);
	SDL_UnlockSurface(sdl_surface);

	sdl_texture = SDL_CreateTextureFromSurface(sdl_renderer, sdl_surface);
	if (sdl_texture == NULL) {
		SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
		return;
	}

	SDL_RenderClear(sdl_renderer);
	SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
	SDL_RenderPresent(sdl_renderer);

	SDL_DestroyTexture(sdl_texture);
}


// Main Render Method
void Engine::render() {
	_surface.fill(COLOR_BLACK);
	
	_surface.fillRect(20, 20, 100, 60, COLOR_RED);
	int LW = 1;
	_surface.drawLine(W/2 + 30, H/2 + 30, W/2 + 30 + 60, H/2 + 30, COLOR_WHITE, LW);
	_surface.drawLine(W/2 + 30, H/2 + 30, W/2 + 30, H/2 + 60 + 30, COLOR_WHITE, LW);
	_surface.drawLine(W/2 - 60, H/2 - 60, W/2 +30 + 60, H/2 + 60 + 30, COLOR_WHITE, LW);
	_surface.drawLine(W/2 + 60, H/2 - 60, W/2 - 30 - 60, H/2 + 60 + 30, COLOR_WHITE, LW);

	_surface.drawTris(W/2, H/2, W/2 - 30, H/2 + 60, W/2 + 30, H/2 + 60, COLOR_GREEN, LW);
	_surface.fillTris(W/2 + 100, H/2, W/2 - 30 + 100, H/2 + 60, W/2 + 30 + 100, H/2 + 60, COLOR_BLUE);
}


// Methods
int Engine::pipeline() {

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	high_resolution_clock::time_point t3;
	high_resolution_clock::time_point t4;

	high_resolution_clock::time_point t5;



	t1 = TIME_PT;
	this->render();
	t2 = TIME_PT;

	this->rasterize();
	t3 = TIME_PT;

	while (isRunning) {
		this->handleEvents();

	}

	// Save the Surface
	t4 = TIME_PT;
	_surface.save_png("Out/img.png");
	t5 = TIME_PT;


	// Logs all the timings
	uint64_t t_render_us = TIME_CAST_US(t2, t1).count();
	uint64_t t_raster_us = TIME_CAST_US(t3, t2).count();
	uint64_t t_save_us   = TIME_CAST_US(t5, t4).count();

	std::cout << "Timers:\n";
	std::cout << "\trender\t " << t_render_us / 1000.f << " ms\n";
	std::cout << "\traster\t " << t_raster_us / 1000.f << " ms\n";
	std::cout << "\tsave\t " << t_save_us / 1000.f << " ms\n";
	return 0;
}
