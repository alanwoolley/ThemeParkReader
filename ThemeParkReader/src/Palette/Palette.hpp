#ifndef PALETTE_H_
#define PALETTE_H_

#include <iostream>

const unsigned PALETTE_SIZE = 256;

typedef struct {
	char red;
	char green;
	char blue;
} PaletteColour;

class Palette {

public:
	static Palette FromFile(std::string palFile);
	PaletteColour colours[PALETTE_SIZE];
	int intensity = 4;

};

#endif