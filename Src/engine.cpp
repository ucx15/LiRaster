#include <iostream>
#include <chrono>
#include <math.h>


#include "engine.hpp"
#include "settings.hpp"
#include "utils.hpp"



#define TIME_PT std::chrono::high_resolution_clock::now()

// Cosntructors and Destructors
Engine::Engine() {
	this->setup();
}

Engine::~Engine() {
	this->quit();
}

void Engine::setup() {
	_buffer = new Color[W*H];
	_surface = Surface(_buffer, W, H);
}

void Engine::quit() {
	// Cleanup
	delete[] _buffer;
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
	// <-------- Rendering -------->
	auto t1 = TIME_PT;
	this->render();

	// Applying ToneMapping
	auto t2 = TIME_PT;
	// _surface.tonemap();

	// Save the Surface
	auto t3 = TIME_PT;
	_surface.save_png("Out/img.png");

	auto t4 = TIME_PT;


	// Logs all the timings
	auto t_render_us  = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	// auto t_encode_us  = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
	auto t_save_us    = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
	auto t_elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t1).count();

	std::cout << "Timers:\n";
	std::cout << "\trender\t " << t_render_us / 1000.f << " ms\n";
	// std::cout << "\tencode\t " << t_encode_us / 1000.f << " ms\n";
	std::cout << "\tsave\t " << t_save_us / 1000.f << " ms\n";
	std::cout << "\nTime Took: \t " << t_elapsed_us / 1000.f << " ms\n";
	return 0;
}
