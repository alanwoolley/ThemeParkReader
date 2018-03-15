#ifndef PALETTE_H_
#define PALETTE_H_

#include <iostream>

namespace reader {
const unsigned PALETTE_SIZE = 256;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} PaletteColour;

class Palette {

public:
	static Palette FromFile(std::string palFile);
	PaletteColour colours[PALETTE_SIZE];
	int intensity = 4;

private:
	static Palette Parse(std::istream& is);
};
}
#endif