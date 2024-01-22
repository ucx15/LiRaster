#include "SDL.h"
#include "engine.hpp"
#include "settings.hpp"
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
	if (SDLWindow == NULL) {
		SDL_Log("SDL_CreateWindow creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

    SDLRenderer =  SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
	if (SDLRenderer == NULL) {
		SDL_Log("SDL_CreateRenderer creation failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDLSurface = SDL_CreateRGBSurface(0, W, H, 32,0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF );
	if (SDLSurface == NULL) {
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
	delete[] ssPoints;
	delete[] tris;

	delete[] enBuffer;

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
	enBuffer = new Color[W*H];
	enSurface = Surface(enBuffer, W, H);

	projMat = glm::perspective(glm::radians(AOV), ASR, EPSILON, FAR_CLIP);

	isRunning = true;
}


void Engine::project() {
	for (int i=0; i<nPoints; i++) {
		Vec3 &in = points[i];     // Input Vector
		Vec3 &out = ssPoints[i];  // Output Vector

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
	for (int i=0; i<nTris*3; i+=3) {
		Vec3 &a = ssPoints[ tris[i]   ];
		Vec3 &b = ssPoints[ tris[i+1] ];
		Vec3 &c = ssPoints[ tris[i+2] ];
		
		enSurface.fillTris(a,b,c, COLOR_BLUE);
		enSurface.drawTris(a,b,c, COLOR_WHITE, 1);
	}

	// Drawing Verticies
	for (int i=0; i<nPoints; i++) {
		enSurface.fillCircle(ssPoints[i], 4, COLOR_RED);
	}
}

void Engine::render() {
	// Copying data to SDL Surface
	enSurface.toU32Surface((uint32_t*)SDLSurface->pixels);

	SDLTexture = SDL_CreateTextureFromSurface(SDLRenderer, SDLSurface);
		
		if (SDLTexture == NULL) {
			SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
			return;
		}

		SDL_RenderClear(SDLRenderer);
		SDL_RenderCopy(SDLRenderer, SDLTexture, NULL, NULL);
		SDL_RenderPresent(SDLRenderer);

	SDL_DestroyTexture(SDLTexture);
}


void Engine::pipeline() {
	this->engineSetup();

	nPoints = 8;
	nTris   = 12;

	points    = new Vec3[nPoints];
	ssPoints = new Vec3[nPoints];
	tris      = new int[3*nTris];

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
	std::cout << "Project\t " << tProjectuS << " us\tRaster\t " << tRasteruS << " us\n";


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
	std::cout << "\nSave\t " << t_save_us / 1000.f << " ms\n";
}
