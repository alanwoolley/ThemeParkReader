#include <cstdint>

namespace reader {

typedef struct
{
	uint32_t dataoffset;
	uint8_t width;
	uint8_t height;
} SpriteFile;

typedef struct
{
	char filename[13];
	uint32_t dataoffset;
	uint32_t datasize;
} MusicFile;

int ExtractSprites(std::string paletteFile, std::string tabFile, std::string datFile, std::string outDir, bool listOnly = false);
int ExtractMusic(std::string tabFile, std::string datFile, std::string outDir, bool listOnly = false);
int ExtractImage(std::string paletteFile, std::string inputFile, std::string outDir);
}