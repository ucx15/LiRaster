#include <iostream>
#include <chrono>
#include <math.h>

#include "SDL.h"
#include "engine.hpp"
#include "settings.hpp"
#include "utils.hpp"


using namespace std::chrono;

#define TIME_PT steady_clock::time_point
#define TIME_NOW() steady_clock::now()
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



// Main Render Method
void Engine::renderSetup() {
	float scale = tan(AOV * 0.5 * M_PI / 180) * EPSILON; 
    float r = ASR * scale;
	float l = -r; 
    float t = scale;
	float b = -t; 

    ProjectionMatrix[0][0] = 2 * EPSILON / (r - l);  
    ProjectionMatrix[1][1] = 2 * EPSILON / (t - b); 
    ProjectionMatrix[2][0] = (r + l) / (r - l); 
    ProjectionMatrix[2][1] = (t + b) / (t - b); 
    ProjectionMatrix[2][2] = -(FAR_CLIP + EPSILON) / (FAR_CLIP - EPSILON); 
    ProjectionMatrix[2][3] = -1; 
    ProjectionMatrix[3][2] = -2 * FAR_CLIP * EPSILON / (FAR_CLIP - EPSILON); 

	buffer = new Color[W*H];
	surface = Surface(buffer, W, H);

	isRunning = true;
}


void Engine::project() {
	float M[4][4];
	memcpy(M, ProjectionMatrix, sizeof(float) * 16);


	for (int i=0; i<n_points; i++) {
		Vec3 &in = points[i];
		Vec3 &out = ss_points[i];

		// Projection
		out.x   = in.x*M[0][0] + in.y*M[1][0] + in.z*M[2][0] + M[3][0]; 
		out.y   = in.x*M[0][1] + in.y*M[1][1] + in.z*M[2][1] + M[3][1]; 
		out.z   = in.x*M[0][2] + in.y*M[1][2] + in.z*M[2][2] + M[3][2]; 
		float w = in.x*M[0][3] + in.y*M[1][3] + in.z*M[2][3] + M[3][3]; 

		if (w != 1) { 
			out.x /= w; 
			out.y /= w; 
			out.z /= w; 
		}

		// Normal Space to Screen Space conversion
		// (-1, 1)  -- x2 ->  (0, 2)  -- /2 ->  (0, 1)  -- xS ->  (0, S)
		out.x = W*(1.f + out.x) / 2.f;
		out.y = H*(1.f - out.y) / 2.f;
	}
}


void Engine::rasterize() {
	// Rendering Triangles from ss_points buffer
	surface.fill(COLOR_BLACK);

	Vec3 a,b,c;

	for (int i=0; i<n_points; i++) {
		surface.fillCircle(ss_points[i], 4, COLOR_RED);
	}

	for (int i=0; i<n_tris*3; i+=3) {
		a = ss_points[ tris[i]   ];
		b = ss_points[ tris[i+1] ];
		c = ss_points[ tris[i+2] ];

		surface.fillTris(a,b,c, COLOR_BLUE);
		surface.drawTris(a,b,c, COLOR_WHITE, 1);
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


int Engine::pipeline() {
	this->renderSetup();

	n_points = 8;
	n_tris   = 12;

	points    = new Vec3[n_points];
	ss_points = new Vec3[n_points];
	tris      = new int[3*n_tris];

	Vec3 pos = Vec3(0, 0, -1.f);
	float rad = .125f;

	// Scene Setup
	{
		points[0] = Vec3( pos.x-rad, pos.y+rad, pos.z+rad); 
		points[1] = Vec3( pos.x-rad, pos.y-rad, pos.z+rad); 
		points[2] = Vec3( pos.x+rad, pos.y-rad, pos.z+rad); 
		points[3] = Vec3( pos.x+rad, pos.y+rad, pos.z+rad);
	
		points[4] = Vec3( pos.x-rad, pos.y+rad, pos.z-rad); 
		points[5] = Vec3( pos.x-rad, pos.y-rad, pos.z-rad); 
		points[6] = Vec3( pos.x+rad, pos.y-rad, pos.z-rad); 
		points[7] = Vec3( pos.x+rad, pos.y+rad, pos.z-rad); 
	}

	// Triangles sequence
	{
		// Back
		tris[0] = 7;
		tris[1] = 6;
		tris[2] = 5;
		tris[3] = 7;
		tris[4] = 5;
		tris[5] = 4;

		// Front
		tris[6] = 0;
		tris[7] = 1;
		tris[8] = 2;
		tris[9] = 0;
		tris[10] = 2;
		tris[11] = 3;

		// Top
		tris[12] = 4;
		tris[13] = 0;
		tris[14] = 3;
		tris[15] = 4;
		tris[16] = 3;
		tris[17] = 7;

		// Bottom
		tris[18] = 1;
		tris[19] = 5;
		tris[20] = 6;
		tris[21] = 1;
		tris[22] = 6;
		tris[23] = 2;


		// Left
		tris[24] = 4;
		tris[25] = 5;
		tris[26] = 1;
		tris[27] = 4;
		tris[28] = 1;
		tris[29] = 0;

		// Right
		tris[30] = 3;
		tris[31] = 2;
		tris[32] = 6;
		tris[33] = 3;
		tris[34] = 6;
		tris[35] = 7;
	}

	TIME_PT t1, t2, t3, t_loop_st, t_loop_en, t5, t6;

	t1 = TIME_NOW();
	this->project();

	t2 = TIME_NOW();
	this->rasterize();
	
	t3 = TIME_NOW();


	uint64_t t_project_us = TIME_CAST_US(t2, t1).count();
	uint64_t t_raster_us  = TIME_CAST_US(t3, t2).count();

	std::cout << "Project\t " << t_project_us << " us\tRaster\t " << t_raster_us << " us\n";


	// Main Loop
	frame_count = 1;
	while (isRunning) {
		this->handleEvents();

		t_loop_st = TIME_NOW();
			this->render();
		t_loop_en = TIME_NOW();

		frame_count++;

		// Logs all the timings
		if ( !(frame_count % (FPS/2)) ) {
			uint64_t t_render_us  = TIME_CAST_US(t_loop_en, t_loop_st).count();

			std::cout << "\tRender\t " << t_render_us / 1000.f << " ms\n";
		}
	}

	// Save the Surface
	t5 = TIME_NOW();
	surface.save_png("Out/img.png");
	t6 = TIME_NOW();


	uint64_t t_save_us   = TIME_CAST_US(t6, t5).count();
	std::cout << "\nSave\t " << t_save_us / 1000.f << " ms\n";

	return 0;
}
