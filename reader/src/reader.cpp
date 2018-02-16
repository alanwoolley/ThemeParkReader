#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <memory>
#include <boost/filesystem.hpp>
#include <FreeImage.h>
#include "palette/palette.hpp"
#include "reader.hpp"

namespace fs = boost::filesystem;

using namespace reader;

int reader::ExtractSprites(std::string paletteFile, std::string tabFile, std::string datFile, std::string outDir, bool listOnly)
{
	FreeImage_Initialise();
	std::cout << FreeImage_GetCopyrightMessage() << std::endl
						<< std::endl;

	std::fstream tabFileIn, datFileIn;
	tabFileIn.open(tabFile.c_str(), std::ios::in | std::ios::binary);

	if (tabFileIn.fail())
	{
		std::cerr << "Could not read .TAB file" << std::endl;
		return (EXIT_FAILURE);
	}

	std::string outPath;
	Palette palette;
	if (!listOnly)
	{
		// Open DAT file
		datFileIn.open(datFile.c_str(), std::ios::in | std::ios::binary);
		if (datFileIn.fail())
		{
			std::cerr << "Could not read .DAT file" << std::endl;
			return (EXIT_FAILURE);
		}

		outPath = std::string(outDir);
		if (outPath.substr(outPath.length() - 1, 1).compare("/") != 0)
		{
			outPath += "/";
		}

		if (paletteFile.length() == 0)
		{
			std::cerr << "Please specify a palette file" << std::endl;
			return (EXIT_FAILURE);
		}
		else
		{
			palette = Palette::FromFile(paletteFile);
		}
	}

	// Move to the first record
	tabFileIn.seekg(0x06, std::ios_base::beg);
	std::vector<SpriteFile> files;
	unsigned counter = 0;
	while (true)
	{
		SpriteFile file;
		tabFileIn.read(reinterpret_cast<char *>(&file.dataoffset), sizeof(file.dataoffset));
		tabFileIn.read(reinterpret_cast<char *>(&file.width), sizeof(file.width));
		tabFileIn.read(reinterpret_cast<char *>(&file.height), sizeof(file.height));
		if (tabFileIn.eof())
		{
			break;
		}
		files.push_back(file);
		std::cout << "File: " << counter;
		std::cout << " (Offset: " << file.dataoffset;
		std::cout << ", Size: " << (uint32_t)file.width << "x" << (uint32_t)file.height << ")" << std::endl;

		counter++;

		if (!listOnly)
		{
			FIBITMAP *bitmap = FreeImage_Allocate(file.width, file.height, 24);

			RGBQUAD pix;
			datFileIn.seekg(file.dataoffset);

			long pos = file.dataoffset;
			unsigned int row = 0;
			unsigned int col = 0;

			while (row < file.height)
			{
				char first;
				datFileIn.seekg(pos, std::ios::beg);
				datFileIn.read(&first, 1);
				pos++;

				if (first < 0)
				{
					col -= first;
				}
				else if (!first)
				{
					col = 0;
					row++;
				}
				else
				{
					for (int i = 0; i < first; i++)
					{
						if (row >= file.height)
							break;
						if (col > file.width)
						{
							row++;
							col = 0;
						}
						else if (col == file.width)
						{
							// do nothing
						}
						else
						{
							char p;
							datFileIn.read(&p, 1);
							pix.rgbRed = palette.colours[(uint8_t)p].red * palette.intensity;
							pix.rgbGreen = palette.colours[(uint8_t)p].green * palette.intensity;
							pix.rgbBlue = palette.colours[(uint8_t)p].blue * palette.intensity;
							FreeImage_SetPixelColor(bitmap, col, file.height - row, &pix);
							pos++;
							col++;
						}
					}
				}
			}

			std::string out = outPath + "sprite-" + std::to_string(counter) + ".bmp";
			if (FreeImage_Save(FIF_BMP, bitmap, out.c_str(), 0))
			{
				std::cout << std::endl
									<< "Image Saved to " << out << std::endl;
			}
			FreeImage_Unload(bitmap);
		}
	}
	tabFileIn.close();
	datFileIn.close();

	FreeImage_DeInitialise();
	return EXIT_SUCCESS;
}

int reader::ExtractMusic(std::string tabFile, std::string datFile, std::string outDir, bool listOnly)
{
	uint32_t datFileSize;
	std::cout << "Extracting..." << std::endl
						<< std::endl;

	std::fstream tabFileIn, datFileIn;
	tabFileIn.open(tabFile.c_str(), std::ios::in | std::ios::binary);

	if (tabFileIn.fail())
	{
		std::cout << "Could not read .TAB file" << std::endl;
		return (EXIT_FAILURE);
	}

	tabFileIn.seekg(0x1a);

	// Grab the size of the .dat file
	tabFileIn.read(reinterpret_cast<char *>(&datFileSize), sizeof(datFileSize));

	std::cout << "File size: " << datFileSize << std::endl;

	// Move to the first record
	tabFileIn.seekg(0x20, std::ios_base::beg);
	int pos = 0;
	std::vector<MusicFile> files;

	std::string outPath;
	if (!listOnly)
	{
		// Open DAT file
		datFileIn.open(datFile.c_str(), std::ios::in | std::ios::binary);
		if (datFileIn.fail())
		{
			std::cout << "Could not read .DAT file" << std::endl;
			return (EXIT_FAILURE);
		}

		outPath = std::string(outDir);
		if (outPath.substr(outPath.length() - 1, 1).compare("/") != 0)
		{
			outPath += "/";
		}
	}

	while (pos < datFileSize)
	{
		MusicFile file;
		tabFileIn.read(file.filename, sizeof(file.filename) - 1);
		tabFileIn.seekg(6, std::ios_base::cur);
		tabFileIn.read(reinterpret_cast<char *>(&file.dataoffset), sizeof(file.dataoffset));
		tabFileIn.seekg(4, std::ios_base::cur);
		tabFileIn.read(reinterpret_cast<char *>(&file.datasize), sizeof(file.datasize));
		tabFileIn.seekg(2, std::ios_base::cur);
		pos += file.datasize;
		files.push_back(file);
		std::cout << "File: " << file.filename;
		std::cout << " (Offset: " << file.dataoffset;
		std::cout << ", Size: " << file.datasize << ")" << std::endl;

		if (!listOnly)
		{
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

	std::cout << std::endl
						<< std::endl
						<< "Done!" << std::endl;

	return EXIT_SUCCESS;
}

int reader::ExtractImage(std::string paletteFile, std::string inputFile, std::string outDir)
{
	FreeImage_Initialise();
	std::cout << FreeImage_GetCopyrightMessage() << std::endl
						<< std::endl;

	std::string outPath = std::string(outDir);
	if (outPath.substr(outPath.length() - 1, 1).compare("/") != 0)
	{
		outPath += "/";
	}

	std::string outName = "image.bmp";

	Palette palette = Palette::FromFile(paletteFile);

	// Load in file

	int width = 320;
	int height = 200;
	FIBITMAP *bitmap = FreeImage_Allocate(width, height, 24);
	std::fstream imageIn;
	RGBQUAD pix;
	imageIn.open(inputFile.c_str(), std::ios::in | std::ios::binary);

	for (int y = height; y > 0; y--)
	{
		for (int x = 0; x < width; x++)
		{
			char colourCode = 0;
			imageIn.read(&colourCode, 1);
			pix.rgbRed = palette.colours[(uint8_t)colourCode].red * palette.intensity;
			pix.rgbGreen = palette.colours[(uint8_t)colourCode].green * palette.intensity;
			pix.rgbBlue = palette.colours[(uint8_t)colourCode].blue * palette.intensity;
			FreeImage_SetPixelColor(bitmap, x, y, &pix);
		}
	}
	imageIn.close();
	std::string out = outPath + outName;
	std::cout << out << std::endl;
	if (FreeImage_Save(FIF_BMP, bitmap, out.c_str(), 0))
	{
		std::cout << std::endl
							<< "Image Saved to " << out << std::endl;
	}

	// Output the pallet

	FIBITMAP *palBit = FreeImage_Allocate(8 * 256, 8, 24);

	for (int x = 0; x < 256; x++)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				pix.rgbBlue = palette.colours[x].blue;
				pix.rgbGreen = palette.colours[x].green;
				pix.rgbRed = palette.colours[x].red;
				FreeImage_SetPixelColor(palBit, (x * 8) + i, j, &pix);
			}
		}
	}
	FreeImage_Save(FIF_BMP, palBit, (outPath + "pal.bmp").c_str(), 0);

	FreeImage_Unload(palBit);
	FreeImage_DeInitialise();

	return EXIT_SUCCESS;
}