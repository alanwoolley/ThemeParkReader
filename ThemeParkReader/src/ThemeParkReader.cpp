#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <FreeImage.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

const std::string APPLICATION_NAME = "ThemeParkReader";

typedef struct {
	int32_t dataoffset;
	uint8_t width;
	uint8_t height;
} SpriteFile;

typedef struct {
	char filename[13];
	int32_t dataoffset;
	int32_t datasize;
} MusicFile;

typedef struct {
	char red;
	char green;
	char blue;
} PaletteColour;

typedef struct {
	PaletteColour colours[256];
	unsigned mult = 4;
} Palette;

void parse_palette(Palette* pal, std::string palFile) {
	std::fstream paletteIn;
	// Load in the palette
	paletteIn.open(palFile.c_str(), std::ios::in | std::ios::binary);

	// Parse the palette

	for (int i = 0; i < 256; i++) {
		PaletteColour col;
		paletteIn.read(&col.red, 1);
		paletteIn.read(&col.green, 1);
		paletteIn.read(&col.blue, 1);
		pal->colours[i] = col;
	}
	paletteIn.close();
}

int extract_sprites(po::variables_map options) {
	FreeImage_Initialise();
	std::cout << FreeImage_GetCopyrightMessage() << std::endl << std::endl;

	std::string tabFile = options["input-file"].as<std::string>();


	if (tabFile.substr(tabFile.length() - 3, 3).compare("TAB") != 0) {
		std::cerr << "Please specify a .TAB input file" << std::endl;
		return EXIT_FAILURE;
	}

	std::string datFile = options["input-file"].as<std::string>();
	datFile.replace(tabFile.length() - 3, 3, "DAT");

	std::fstream tabFileIn, datFileIn;
	tabFileIn.open(tabFile.c_str(), std::ios::in | std::ios::binary);

	if (tabFileIn.fail()) {
		std::cerr << "Could not read .TAB file" << std::endl;
		return(EXIT_FAILURE);
	}

	std::string outPath;
	Palette palette;
	if (!options.count("list")) {
		// Open DAT file
		datFileIn.open(datFile.c_str(), std::ios::in | std::ios::binary);
		if (datFileIn.fail()) {
			std::cerr << "Could not read .DAT file" << std::endl;
			return(EXIT_FAILURE);
		}

		outPath = options["output-dir"].as<std::string>();
		if (outPath.substr(outPath.length() - 1, 1).compare("/") != 0) {
			outPath += "/";
		}

		if (!options.count("palette")) {
			std::cerr << "Please specify a palette file" << std::endl;
			return (EXIT_FAILURE);
		}
		else {
			std::string palPath = options["palette"].as<std::string>();
			parse_palette(&palette, palPath);
		}
	}


	// Move to the first record
	tabFileIn.seekg(0x06, std::ios_base::beg);
	std::vector<SpriteFile> files;
	unsigned counter = 0;
	while (true) {
		SpriteFile file;
		tabFileIn.read(reinterpret_cast<char*>(&file.dataoffset), sizeof(file.dataoffset));
		tabFileIn.read(reinterpret_cast<char*>(&file.width), sizeof(file.width));
		tabFileIn.read(reinterpret_cast<char*>(&file.height), sizeof(file.height));
		if (tabFileIn.eof()) {
			break;
		}
		files.push_back(file);
		std::cout << "File: " << counter;
		std::cout << " (Offset: " << file.dataoffset;
		std::cout << ", Size: " << (uint32_t)file.width << "x" << (uint32_t)file.height << ")" << std::endl;

		counter++;

		if (!options.count("list")) {
			FIBITMAP* bitmap = FreeImage_Allocate(file.width, file.height, 24);

			RGBQUAD pix;
			datFileIn.seekg(file.dataoffset);

			long pos = file.dataoffset;
			unsigned int row = 0;
			unsigned int col = 0;

			while (row < file.height) {
				char first;
				datFileIn.seekg(pos, std::ios::beg);
				datFileIn.read(&first, 1);
				pos++;

				if (first < 0) {
					col -= first;
				}
				else if (!first) {
					col = 0;
					row++;
				}
				else {
					for (int i = 0; i < first; i++) {
						if (row >= file.height) break;
						if (col > file.width) {
							row++;
							col = 0;
						}
						else if (col == file.width) {
							// do nothing
						}
						else {
							char p;
							datFileIn.read(&p, 1);
							pix.rgbRed = palette.colours[(uint8_t)p].red * palette.mult;
							pix.rgbGreen = palette.colours[(uint8_t)p].green * palette.mult;
							pix.rgbBlue = palette.colours[(uint8_t)p].blue * palette.mult;
							FreeImage_SetPixelColor(bitmap, col, file.height - row, &pix);
							pos++;
							col++;
						}

					}
				}
			}

			std::string out = outPath + "sprite-" + std::to_string(counter) + ".bmp";
			if (FreeImage_Save(FIF_BMP, bitmap, out.c_str(), 0)) {
				std::cout << std::endl << "Image Saved to " << out << std::endl;
			}
			FreeImage_Unload(bitmap);
		}
	}
	tabFileIn.close();
	datFileIn.close();

	FreeImage_DeInitialise();
	return EXIT_SUCCESS;
}

int extract_music(po::variables_map options) {
	std::string tabFile = options["input-file"].as<std::string>();



	if (tabFile.substr(tabFile.length() - 3, 3).compare("TAB") != 0) {
		std::cerr << "Please specify a .TAB input file" << std::endl;
		return EXIT_FAILURE;
	}

	std::string datFile = options["input-file"].as<std::string>();
	datFile.replace(tabFile.length() - 3, 3, "DAT");


	int32_t datFileSize;
	std::cout << "Extracting..." << std::endl << std::endl;


	std::fstream tabFileIn, datFileIn;
	tabFileIn.open(tabFile.c_str(), std::ios::in | std::ios::binary);

	if (tabFileIn.fail()) {
		std::cout << "Could not read .TAB file" << std::endl;
		return(EXIT_FAILURE);
	}

	tabFileIn.seekg(0x1a);

	// Grab the size of the .dat file
	tabFileIn.read(reinterpret_cast<char*>(&datFileSize), sizeof(datFileSize));

	std::cout << "File size: " << datFileSize << std::endl;

	// Move to the first record
	tabFileIn.seekg(0x20, std::ios_base::beg);
	int pos = 0;
	std::vector<MusicFile> files;


	std::string outPath;
	if (!options.count("list")) {
		// Open DAT file
		datFileIn.open(datFile.c_str(), std::ios::in | std::ios::binary);
		if (datFileIn.fail()) {
			std::cout << "Could not read .DAT file" << std::endl;
			return(EXIT_FAILURE);
		}

		outPath = options["output-dir"].as<std::string>();
		if (outPath.substr(outPath.length() - 1, 1).compare("/") != 0) {
			outPath += "/";
		}
	}

	while (pos < datFileSize) {
		MusicFile file;
		tabFileIn.read(file.filename, sizeof(file.filename) - 1);
		tabFileIn.seekg(6, std::ios_base::cur);
		tabFileIn.read(reinterpret_cast<char*>(&file.dataoffset), sizeof(file.dataoffset));
		tabFileIn.seekg(4, std::ios_base::cur);
		tabFileIn.read(reinterpret_cast<char*>(&file.datasize), sizeof(file.datasize));
		tabFileIn.seekg(2, std::ios_base::cur);
		pos += file.datasize;
		files.push_back(file);
		std::cout << "File: " << file.filename;
		std::cout << " (Offset: " << file.dataoffset;
		std::cout << ", Size: " << file.datasize << ")" << std::endl;

		if (!options.count("list")) {
			// Seek to offset
			datFileIn.seekg(file.dataoffset);
			std::unique_ptr<char> datData(new char[file.datasize]);

			// Read the data from the dat file
			datFileIn.read(datData.get(), file.datasize);

			std::fstream dataOut;
			dataOut.open((outPath + file.filename).c_str(), std::ios::out | std::ios::binary);
			dataOut.write(datData.get(), file.datasize);
			dataOut.close();
		}


	}

	tabFileIn.close();
	datFileIn.close();

	std::cout << std::endl << std::endl << "Done!" << std::endl;

	return EXIT_SUCCESS;
}

int extract_image(po::variables_map options) {
	FreeImage_Initialise();
	std::cout << FreeImage_GetCopyrightMessage() << std::endl << std::endl;

	std::string paletteFile = options["palette"].as<std::string>();
	std::string imageFile = options["input-file"].as<std::string>();

	std::string outPath = options["output-dir"].as<std::string>();

	if (outPath.substr(outPath.length() - 1, 1).compare("/") != 0) {
		outPath += "/";
	}

	std::string outName = "image.bmp";

	Palette palette;
	parse_palette(&palette, paletteFile);

	// Load in file

	int width = 320;
	int height = 200;
	FIBITMAP *bitmap = FreeImage_Allocate(width, height, 24);
	std::fstream imageIn;
	RGBQUAD pix;
	imageIn.open(imageFile.c_str(), std::ios::in | std::ios::binary);

	for (int y = height; y > 0; y--) {
		for (int x = 0; x < width; x++) {
			char colourCode = 0;
			imageIn.read(&colourCode, 1);
			pix.rgbRed = palette.colours[(uint8_t)colourCode].red * palette.mult;
			pix.rgbGreen = palette.colours[(uint8_t)colourCode].green * palette.mult;
			pix.rgbBlue = palette.colours[(uint8_t)colourCode].blue * palette.mult;
			FreeImage_SetPixelColor(bitmap, x, y, &pix);
		}
	}
	imageIn.close();
	std::string out = outPath + outName;
	std::cout << out << std::endl;
	if (FreeImage_Save(FIF_BMP, bitmap, out.c_str(), 0)) {
		std::cout << std::endl << "Image Saved to " << out << std::endl;
	}

	// Output the pallet

	FIBITMAP *palBit = FreeImage_Allocate(8 * 256, 8, 24);

	for (int x = 0; x < 256; x++) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				pix.rgbBlue = palette.colours[x].blue;
				pix.rgbGreen = palette.colours[x].green;
				pix.rgbRed = palette.colours[x].red;
				FreeImage_SetPixelColor(palBit, (x * 8) + i, j, &pix);
			}
		}
	}
	FreeImage_Save(FIF_BMP, palBit, (outPath + "pal.bmp").c_str(), 0);

	FreeImage_DeInitialise();

	return EXIT_SUCCESS;
}

void print_usage(std::string name, const po::options_description &options) {
	std::cout << "Usage: " << name << " [options] <input file>" << std::endl;
	std::cout << "Manipulates Bullfrog Theme Park data files" << std::endl;
	std::cout << options << std::endl;
	std::cout << "This software comes with absolutely no warranty." << std::endl;
}
int main(int argc, const char* argv[]) {


	po::options_description usage("Generic");
	usage.add_options()
		("help", "display this message");

	po::options_description actions("Actions");
	actions.add_options()
		("extract-sounds,s", "Extract music or sounds")
		("extract-image,i", "Extract fullscreen image")
		("extract-sprites,r", "Extract sprites");

	po::options_description modifiers("Modifiers");
	modifiers.add_options()
		("list,l", "Only list files - do NOT extract")
		("output-dir,o", po::value<std::string>(), "Output directory")
		("palette,p", po::value<std::string>(), "Image palette file");

	po::options_description hidden("Hidden Options");
	hidden.add_options()
		("input-file", po::value<std::string>(), "Input file");

	po::options_description options_desc;
	options_desc.add(usage).add(actions).add(modifiers).add(hidden);

	po::options_description visible;
	visible.add(usage).add(actions).add(modifiers);

	po::positional_options_description p;
	p.add("input-file", -1);

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(options_desc).positional(p).run(), vm);
		po::notify(vm);
	}
	catch (po::error &e) {
		std::cerr << "Error parsing command-line options: " << e.what() << std::endl << std::endl;
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}

	bool validAction = vm.count("extract-sounds") ^ vm.count("extract-image") ^ vm.count("extract-sprites");

	if (vm.count("help") || vm.size() == 0) {
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}

	if (!vm.count("list") && !vm.count("output-dir")) {
		std::cerr << "Output directory not specified" << std::endl << std::endl;
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}

	if (vm.count("extract-sounds") && vm.count("input-file")) {
		return extract_music(vm);
	}

	if (vm.count("extract-image") && vm.count("input-file") && vm.count("palette")) {
		return extract_image(vm);
	}

	if (vm.count("extract-sprites") && vm.count("input-file")) {
		return extract_sprites(vm);
	}

	if (validAction && !vm.count("input-file")) {
		std::cerr << "Please give an input file" << std::endl << std::endl;
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;

}
