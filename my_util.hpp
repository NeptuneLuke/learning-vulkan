#pragma once

#include <string>
#include <vector>


namespace my_util {


enum Color {
	Black, Red, Green, Yellow, Blue, Magenta, Cyan, White,
	Bright_Black, Bright_Red, Bright_Green, Bright_Yellow,
	Bright_Blue, Bright_Magenta, Bright_Cyan, Bright_White
};


void LOG_MESSAGE(std::string message, Color text, Color background, uint16_t indentation_width);


std::vector<char> read_file(const std::string& file_path);


} // namespace my_util