#include <vulkan/vulkan.h>

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <cstdlib>		// miscellaneous utilities (EXIT_SUCCESS, EXIT_FAILURE)

/*
 * This class stores the Vulkan objects
 * as private members and interacts with them
 * through functions
*/
class HelloTriangle {

public:

	void run() {

		// If at any time during execution an error occurs,
		// we'll throw a std::runtime_error exception
		// which will propagate back to the main function and catched
		// by the general std::exception

		init_vulkan();

		main_loop();

		cleanup();
	}


private:

	// Initialize the Vulkan objects
	void init_vulkan() {

	}

	// Iterates render operations until the window is closed
	void main_loop() {

	}

	// Deallocate resources
	void cleanup() {

	}

};


int main() {

	HelloTriangle application;

	try {

		application.run();
	}
	catch (const std::exception& ex) {

		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}