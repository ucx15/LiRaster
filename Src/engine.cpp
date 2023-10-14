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


	buffer = new Color[W*H];

	surface = Surface(buffer, W, H);
	isRunning = true;
}


void Engine::quit() {
	// Cleanup
	delete[] buffer;

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


// Main Render Method
void Engine::render() {

	// Normal Space to Screen Space conversion
	Vec3 point;


	for (int i=0; i<3; i++) {
		point = points[i];
		// (-1, 1)  -- x2 ->  (0, 2)  -- /2 ->  (0, 1)  -- xS ->  (0, S)
		ss_points[i] = Vec3( W*(1+point.x)/2, H*(1-point.y)/2, 0 );
	}


	// Rendering 
	surface.fill(COLOR_BLACK);

	Vec3 a,b,c;
	for (int i=0; i<3; i+=3) {
		a = ss_points[ tris[i]   ];
		b = ss_points[ tris[i+1] ];
		c = ss_points[ tris[i+2] ];

		surface.drawLine(a, b, COLOR_WHITE, 1);
		surface.drawLine(b, c, COLOR_WHITE, 1);
		surface.drawLine(a, c, COLOR_WHITE, 1);
	}
		
	for (int i=0; i<3; i++) {
		surface.fillRect(ss_points[i].x, ss_points[i].y, 10, 10, COLOR_RED);
	}


	// surface.fillRect(20, 20, 100, 60, COLOR_RED);
	// int LineWidth = 1;
	// surface.drawLine(W/2 + 30, H/2 + 30, W/2 + 30 + 60, H/2 + 30, COLOR_WHITE, LineWidth);
	// surface.drawLine(W/2 + 30, H/2 + 30, W/2 + 30, H/2 + 60 + 30, COLOR_WHITE, LineWidth);
	// surface.drawLine(W/2 - 60, H/2 - 60, W/2 +30 + 60, H/2 + 60 + 30, COLOR_WHITE, LineWidth);
	// surface.drawLine(W/2 + 60, H/2 - 60, W/2 - 30 - 60, H/2 + 60 + 30, COLOR_WHITE, LineWidth);

	// surface.drawTris(W/2, H/2, W/2 - 30, H/2 + 60, W/2 + 30, H/2 + 60, COLOR_GREEN, LineWidth);
	// surface.fillTris(W/2 + 100, H/2, W/2 - 30 + 100, H/2 + 60, W/2 + 30 + 100, H/2 + 60, COLOR_BLUE);


	// Copying data to SDL Surface
	SDL_LockSurface(sdl_surface);
		surface.toU32Surface((uint32_t*)sdl_surface->pixels);
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


// Methods
int Engine::pipeline() {

	high_resolution_clock::time_point t1, t2, t3, t4;
	
	points = new Vec3[3];
	ss_points = new Vec3[3];
	tris = new int[3 * 1];

	points[0] = Vec3( 0, .5, 0);
	points[1] = Vec3( .5, 0, 0);
	points[2] = Vec3(-.5, 0, 0);

	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;


	while (isRunning) {
		this->handleEvents();

		t1 = TIME_PT;
		this->render();
		t2 = TIME_PT;


		// Logs all the timings
		uint64_t t_render_us = TIME_CAST_US(t2, t1).count();

		std::cout << "\trender\t " << t_render_us / 1000.f << " ms\n";
	}

	// Save the Surface
	t3 = TIME_PT;
	surface.save_png("Out/img.png");
	t4 = TIME_PT;

	delete[] points;
	delete[] ss_points;
	delete[] tris;

	uint64_t t_save_us   = TIME_CAST_US(t4, t3).count();
	std::cout << "\tsave\t " << t_save_us / 1000.f << " ms\n";

	return 0;
}
