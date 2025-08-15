#include "engine.hpp"


int main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

	Engine LiRasterEngine = Engine();
	LiRasterEngine.pipeline();

	return EXIT_SUCCESS;
}
