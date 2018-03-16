#include "palette.hpp"
#include <fstream>

namespace reader {

Palette Palette::FromFile(std::string palFile) {
	std::fstream paletteIn;
	// Load in the palette
	paletteIn.open(palFile.c_str(), std::ios::in | std::ios::binary);
	Palette pal = Palette::Parse(paletteIn);
	paletteIn.close();
	return pal;
}

Palette Palette::Parse(std::istream& is) {
Palette pal;
	// Parse the palette
	for (int i = 0; i < PALETTE_SIZE; i++) {
		PaletteColour col;
		is.read(reinterpret_cast<char*>(&col.red), 1);
		is.read(reinterpret_cast<char*>(&col.green), 1);
		is.read(reinterpret_cast<char*>(&col.blue), 1);
		if (is.good()) {
			pal.colours[i] = col;
		} else {
			break;
		}
	}

	return pal;
}

}
