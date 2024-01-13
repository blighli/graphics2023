#include "application.h"

int main() {
	initObjects();
	Application application;
	try {
		application.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}