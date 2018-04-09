#include "tab.hpp"
#include <fstream>
#include <iostream>

namespace reader {

// FontTab ParseFontTab(std::istream& tabFileIn) {
// 	std::vector<FontCharacter> characters;

// 	tabFileIn.seekg(0x06, std::ios_base::beg);

// 	while (!tabFileIn.eof()) {
// 		FontCharacter fc;
// 		tabFileIn.read(reinterpret_cast<char *>(&fc.dataoffset), sizeof(fc.dataoffset));
// 		tabFileIn.read(reinterpret_cast<char *>(&fc.width), sizeof(fc.width));
// 		tabFileIn.read(reinterpret_cast<char *>(&fc.height), sizeof(fc.height));

// 		if (tabFileIn.good()) {
// 			characters.push_back(fc);
// 		}
// 	}

// 	return FontTab(characters);
// }

SpriteTab ParseSpriteTab(std::istream& tabFileIn) {
  std::vector<SpriteFile> sprites;

	// Move to the first record
	tabFileIn.seekg(0x06, std::ios_base::beg);
  uint counter = 0;
  while (!tabFileIn.eof())
	{
		SpriteFile file;
		tabFileIn.read(reinterpret_cast<char *>(&file.dataoffset), sizeof(file.dataoffset));
		tabFileIn.read(reinterpret_cast<char *>(&file.width), sizeof(file.width));
		tabFileIn.read(reinterpret_cast<char *>(&file.height), sizeof(file.height));

		if (tabFileIn.good()) {
			sprites.push_back(file);
			std::cout << "File: " << counter;
			std::cout << " (Offset: " << file.dataoffset;
			std::cout << ", Size: " << (uint32_t)file.width << "x" << (uint32_t)file.height << ")" << std::endl;

			counter++;
		}
  }
  
  return SpriteTab(sprites);
}

}