#include <iostream>

#include "engine.hpp"


void parseArgs(int nArgs, char *args[]) {
	if (nArgs > 1) {
		for (int i=0; i<nArgs; i++) {
			std::cout << args[i] << '\n';
		}
	}
}


int main(int argc, char *argv[]) {
	parseArgs(argc, argv);

	Engine Runner = Engine();
	return Runner.pipeline();
}
