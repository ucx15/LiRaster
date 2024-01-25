#include "SDL.h"
#include "engine.hpp"
#include "settings.hpp"

// #define TRACK_MEMORY    // Can be used to Track Allocated and Deallocated memory 
#include "utils.hpp"


// Cosntructors and Destructors
Engine::Engine() {
	this->SDLSetup();
}

Engine::~Engine() {
	this->quit();
}

void Engine::SDLSetup() {
    SDLWindow = SDL_CreateWindow("LiRaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
	if ( !SDLWindow ) {
		SDL_Log("SDL_CreateWindow creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

    SDLRenderer =  SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
	if ( !SDLRenderer ) {
		SDL_Log("SDL_CreateRenderer creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDLTexture = SDL_CreateTexture(SDLRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, W, H);
	if ( !SDLTexture ) {
		SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if(SDL_Init(SDL_INIT_EVERYTHING)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}


void Engine::quit() {
	// Cleanup
	MEM_DEALLOC(enTriIndex,      3*enTriCount);
	MEM_DEALLOC(enSSVerticies,   enVxCount);
	MEM_DEALLOC(enVerticies,     enVxCount);
	MEM_DEALLOC(enBuffer, 		 W*H);
	MEM_DEALLOC(enTextureBuffer, W*H);


	SDL_DestroyTexture(SDLTexture);
	SDL_DestroyRenderer(SDLRenderer);
	SDL_DestroyWindow(SDLWindow);
	SDL_Quit();
}


void Engine::handleEvents() {
	while (SDL_PollEvent(&SDLEvent)) {
		if (SDLEvent.type == SDL_QUIT) {
			isRunning = false;
		}
	}
}


// Main Render Method
void Engine::engineSetup() {
	MEM_ALLOC(enTextureBuffer, uint32_t, W*H);
	MEM_ALLOC(enBuffer, Color,W*H);	

	enSurface = Surface(enBuffer, W, H);
	projMat = glm::perspective(glm::radians(AOV), ASR, EPSILON, FAR_CLIP);

	isRunning = true;
}


void Engine::project() {
	for (int i=0; i<enVxCount; i++) {
		Vec3 &in = enVerticies[i];     // Input Vector
		Vec3 &out = enSSVerticies[i];  // Output Vector

		Vec4 intr = projMat * Vec4(in, 1.0f);

        // Normalize intr coordinates
        if (intr.w != 0) {
            out.x = intr.x/intr.w;
            out.y = intr.y/intr.w;
            out.z = intr.z/intr.w;
        }

		// Normal Space to Screen Space conversion
		// (-1, 1)  -- x2 ->  (0, 2)  -- /2 ->  (0, 1)  -- xS ->  (0, S)
		out.x = W * (1.f+out.x)/2.f;
		out.y = H * (1.f-out.y)/2.f;
	}
}

void Engine::rasterize() {
	// Rendering Triangles from ss_points buffer
	enSurface.fill(COLOR_BLACK);

	// Drawing Triangles
	for (int i=0; i<enTriCount*3; i+=3) {
		Vec3 &a = enSSVerticies[ enTriIndex[i]   ];
		Vec3 &b = enSSVerticies[ enTriIndex[i+1] ];
		Vec3 &c = enSSVerticies[ enTriIndex[i+2] ];
		
		enSurface.fillTris(a,b,c, COLOR_BLUE);
		enSurface.drawTris(a,b,c, COLOR_WHITE, 1);
	}

	// Drawing Verticies
	for (int i=0; i<enVxCount; i++) {
		enSurface.fillCircle(enSSVerticies[i], 4, COLOR_RED);
	}
}

void Engine::render() {
	// Copying data to 32 bit buffer
	enSurface.toU32Surface(enTextureBuffer);

	// Copying data to VRAM
	SDL_UpdateTexture(SDLTexture, NULL, enTextureBuffer, W*4);	
	if ( !SDLTexture ) {
		SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
		return;
	}

	// Presenting to Display device
	SDL_RenderClear(SDLRenderer);
	SDL_RenderCopy(SDLRenderer, SDLTexture, NULL, NULL);
	SDL_RenderPresent(SDLRenderer);
}

void Engine::loadScene() {
	enVxCount = 8;
	enTriCount = 12;

	MEM_ALLOC(enVerticies, Vec3, enVxCount);
	MEM_ALLOC(enSSVerticies, Vec3, enVxCount);
	MEM_ALLOC(enTriIndex, int, 3*enTriCount);

	Vec3 pos = Vec3(0, 0, -1.f);
	float rad = .125f;

	// Scene Setup
	{
		enVerticies[0] = Vec3( pos.x-rad, pos.y+rad, pos.z+rad); 
		enVerticies[1] = Vec3( pos.x-rad, pos.y-rad, pos.z+rad); 
		enVerticies[2] = Vec3( pos.x+rad, pos.y-rad, pos.z+rad); 
		enVerticies[3] = Vec3( pos.x+rad, pos.y+rad, pos.z+rad);
	
		enVerticies[4] = Vec3( pos.x-rad, pos.y+rad, pos.z-rad); 
		enVerticies[5] = Vec3( pos.x-rad, pos.y-rad, pos.z-rad); 
		enVerticies[6] = Vec3( pos.x+rad, pos.y-rad, pos.z-rad); 
		enVerticies[7] = Vec3( pos.x+rad, pos.y+rad, pos.z-rad); 
	}

	// Triangles sequence
	{
		// Back
		enTriIndex[0] = 7;
		enTriIndex[1] = 6;
		enTriIndex[2] = 5;
		enTriIndex[3] = 7;
		enTriIndex[4] = 5;
		enTriIndex[5] = 4;

		// Front
		enTriIndex[6] = 0;
		enTriIndex[7] = 1;
		enTriIndex[8] = 2;
		enTriIndex[9] = 0;
		enTriIndex[10] = 2;
		enTriIndex[11] = 3;

		// Top
		enTriIndex[12] = 4;
		enTriIndex[13] = 0;
		enTriIndex[14] = 3;
		enTriIndex[15] = 4;
		enTriIndex[16] = 3;
		enTriIndex[17] = 7;

		// Bottom
		enTriIndex[18] = 1;
		enTriIndex[19] = 5;
		enTriIndex[20] = 6;
		enTriIndex[21] = 1;
		enTriIndex[22] = 6;
		enTriIndex[23] = 2;


		// Left
		enTriIndex[24] = 4;
		enTriIndex[25] = 5;
		enTriIndex[26] = 1;
		enTriIndex[27] = 4;
		enTriIndex[28] = 1;
		enTriIndex[29] = 0;

		// Right
		enTriIndex[30] = 3;
		enTriIndex[31] = 2;
		enTriIndex[32] = 6;
		enTriIndex[33] = 3;
		enTriIndex[34] = 6;
		enTriIndex[35] = 7;
	}
}


void Engine::pipeline() {
	// Engine Class Startup code
	this->engineSetup();


	// Loading Scene into Memory
	this->loadScene();


	TIME_PT tPtProject1, tPtProject2, tPtRaster1, tPtRaster2;

	// Projection
	tPtProject1 = TIME_NOW();
	this->project();
	tPtProject2 = TIME_NOW();

	// Rasterization
	tPtRaster1 = TIME_NOW();
	this->rasterize();
	tPtRaster2 = TIME_NOW();

	// Logging
	auto tProjectuS = TIME_DUR(tPtProject2, tPtProject1);
	auto tRasteruS  = TIME_DUR(tPtRaster2, tPtRaster1);
	std::cout << "\nProject\t " << tProjectuS << " us\tRaster\t " << tRasteruS << " us\n";


	float lastLogTime = 0.f;
	TIME_PT tPtRender1, tPtRender2, tDt1, tDt2;

	tDt1 = TIME_NOW();

	// Main Loop
	while (isRunning) {

		// Calculate delta time
		{
			tDt2 = TIME_NOW();
			deltaTime = TIME_DUR(tDt2, tDt1)/1E6F;
			tDt1 = TIME_NOW();
		}


		// Handle Events
		this->handleEvents();


		// Render
		tPtRender1 = TIME_NOW();
			this->render();
		tPtRender2 = TIME_NOW();

		lastLogTime += deltaTime;

		// Logs all the timings
		if ( lastLogTime>UPDATE_TIME ) {
			lastLogTime = 0.f;

			auto tRender = TIME_DUR(tPtRender2, tPtRender1);
			std::cout << "FPS " << 1/deltaTime << "\tRender  " << tRender/1E3F << " ms\tdt " << deltaTime*1E3F << " ms\n";
		}
	}


	TIME_PT tPtSave1, tPtSave2; 
	// Save the Surface
	tPtSave1 = TIME_NOW();
	enSurface.savePNG("Out/img.png");
	tPtSave2 = TIME_NOW();

	uint64_t t_save_us   = TIME_DUR(tPtSave2, tPtSave1);
	std::cout << "\nSave\t " << t_save_us / 1000.f << " ms\n\n";
}
