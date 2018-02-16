#include "palette.hpp"
#include <fstream>

using namespace reader;

Palette reader::Palette::FromFile(std::string palFile) {
	Palette pal;
	std::fstream paletteIn;
	// Load in the palette
	paletteIn.open(palFile.c_str(), std::ios::in | std::ios::binary);

	// Parse the palette

	for (int i = 0; i < PALETTE_SIZE; i++) {
		PaletteColour col;
		paletteIn.read(&col.red, 1);
		paletteIn.read(&col.green, 1);
		paletteIn.read(&col.blue, 1);
		pal.colours[i] = col;
	}
	paletteIn.close();
	return pal;
}
