#include "engine.hpp"


int main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

	Engine Runner = Engine();
	Runner.pipeline();

	return EXIT_SUCCESS;
}
