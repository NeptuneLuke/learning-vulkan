#include "my_util.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>


namespace my_util {


void LOG_MESSAGE(std::string message, Color text, Color background, uint16_t indentation_width) {

	// Default white text on black background: \033[0m
	std::string colors = "\033[0m";

	switch (text) {
		default:
		case Color::Black: { colors = "\033[30;"; break; }
		case Color::Red: { colors = "\033[31;"; break; }
		case Color::Green: { colors = "\033[32;"; break; }
		case Color::Yellow: { colors = "\033[33;"; break; }
		case Color::Blue: { colors = "\033[34;"; break; }
		case Color::Magenta: { colors = "\033[35;"; break; }
		case Color::Cyan: { colors = "\033[36;"; break; }
		case Color::White: { colors = "\033[37;"; break; }

		case Color::Bright_Black: { colors = "\033[90;"; break; }
		case Color::Bright_Red: { colors = "\033[91;"; break; }
		case Color::Bright_Green: { colors = "\033[92;"; break; }
		case Color::Bright_Yellow: { colors = "\033[93;"; break; }
		case Color::Bright_Blue: { colors = "\033[94;"; break; }
		case Color::Bright_Magenta: { colors = "\033[95;"; break; }
		case Color::Bright_Cyan: { colors = "\033[96;"; break; }
		case Color::Bright_White: { colors = "\033[97;"; break; }
	}

	switch (background) {
		default:
		case Color::Black: { colors += "40m"; break; }
		case Color::Red: { colors += "41m"; break; }
		case Color::Green: { colors += "42m"; break; }
		case Color::Yellow: { colors += "43m"; break; }
		case Color::Blue: { colors += "44m"; break; }
		case Color::Magenta: { colors += "45m"; break; }
		case Color::Cyan: { colors += "46m"; break; }
		case Color::White: { colors += "47m"; break; }

		case Color::Bright_Black: { colors += "100m"; break; }
		case Color::Bright_Red: { colors += "101m"; break; }
		case Color::Bright_Green: { colors += "102m"; break; }
		case Color::Bright_Yellow: { colors += "103m"; break; }
		case Color::Bright_Blue: { colors += "104m"; break; }
		case Color::Bright_Magenta: { colors += "105m"; break; }
		case Color::Bright_Cyan: { colors += "106m"; break; }
		case Color::Bright_White: { colors += "107m"; break; }
	}

	if (indentation_width > 0) {
		std::cout << colors << std::setfill(' ') << std::setw(indentation_width) << ' ' << message << "\033[0m \n";
	}
	else {
		// else do not indent text
		std::cout << colors << message << "\033[0m \n";
	}

}


std::vector<char> read_file(const std::string& file_path) {

	// Read the file in binary mod starting at the end of the file
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to open file! \033[0m \n");
	}

	// Get file size and create a buffer for the file's contents
	size_t file_size = (size_t) file.tellg();
	std::vector<char> file_buffer(file_size);

	// Go back at the beginning of the file and read all bytes
	file.seekg(0);
	file.read(file_buffer.data(), file_size);

	file.close();

	return file_buffer;
}

} // namespace my_util