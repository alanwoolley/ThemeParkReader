#include "tab.hpp"
#include <fstream>
#include <iostream>

namespace reader {

SpriteTab ParseTest(std::istream& tabIn) {
tabIn.seekg(0x06, std::ios_base::beg);
char c = 0x00;
while(!tabIn.eof()) {

	if (tabIn.read(&c, sizeof(char))) {
		printf("char: 0x%02X\n", (unsigned int)(c & 0xFF));
	}
}
};


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