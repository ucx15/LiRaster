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
	delete[] points;
	delete[] ss_points;
	delete[] tris;

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


void Engine::project() {
	// Normal Space to Screen Space conversion
	for (int i=0; i<n_points; i++) {
		Vec3 &point = points[i];
		// (-1, 1)  -- x2 ->  (0, 2)  -- /2 ->  (0, 1)  -- xS ->  (0, S)
		ss_points[i] = Vec3( W*(1.f + point.x/ASR) / 2.f , H*(1.f-point.y)/2.f, 0 );
	}
}


// Main Render Method
void Engine::rasterize() {
	// Rendering Triangles from ss_points buffer
	surface.fill(COLOR_BLACK);

	Vec3 a,b,c;
	for (int i=0; i<n_tris*3; i+=3) {
		a = ss_points[ tris[i]   ];
		b = ss_points[ tris[i+1] ];
		c = ss_points[ tris[i+2] ];
		
		// surface.fillTris(a, b, c, COLOR_GREEN);

		surface.drawLine(a, b, COLOR_WHITE, 1);
		surface.drawLine(b, c, COLOR_WHITE, 1);
		surface.drawLine(a, c, COLOR_WHITE, 1);
	}

	for (int i=0; i<n_points; i++) {
		surface.fillCircle(ss_points[i], 4, COLOR_RED);
	}

}


void Engine::render() {
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

	n_points = 4;
	n_tris   = 2;

	points    = new Vec3[n_points];
	ss_points = new Vec3[n_points];
	tris      = new int[3*n_tris];

	// Scene Setup
	points[0] = Vec3( -.25, .25, 0);
	points[1] = Vec3( -.25, -.25, 0);
	points[2] = Vec3( +.25, -.25, 0);
	points[3] = Vec3( +.25, .25, 0);

	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;

	tris[3] = 0;
	tris[4] = 2;
	tris[5] = 3;


	high_resolution_clock::time_point t1, t2, t3, t_loop_st, t_loop_en, t5, t6;

	// Main Loop
	frame_count = 1;	
	t1 = TIME_PT;
	this->project();

	t2 = TIME_PT;
	this->rasterize();
	t3 = TIME_PT;

	uint64_t t_project_us = TIME_CAST_US(t2, t1).count();
	uint64_t t_raster_us  = TIME_CAST_US(t3, t2).count();

	std::cout << "Project\t " << t_project_us << " us\tRaster\t " << t_raster_us << " us\n";


	while (isRunning) {
		this->handleEvents();

		t_loop_st = TIME_PT;
			this->render();
		t_loop_en = TIME_PT;

		frame_count++;

		// Logs all the timings
		if ( !(frame_count % (FPS/2)) ) {
			uint64_t t_render_us  = TIME_CAST_US(t_loop_en, t_loop_st).count();

			std::cout << "\tRender\t " << t_render_us / 1000.f << " ms\n";
		}
	}

	// Save the Surface
	t5 = TIME_PT;
	surface.save_png("Out/img.png");
	t6 = TIME_PT;


	uint64_t t_save_us   = TIME_CAST_US(t6, t5).count();
	std::cout << "\nSave\t " << t_save_us / 1000.f << " ms\n";

	return 0;
}
