#ifndef READER_H_
#define READER_H_

#include <cstdint>

namespace reader {
  int ExtractSprites(std::string paletteFile, std::string tabFile, std::string datFile, std::string outDir, bool listOnly = false);
  int ExtractMusic(std::string tabFile, std::string datFile, std::string outDir, bool listOnly = false);
  int ExtractImage(std::string paletteFile, std::string inputFile, std::string outDir);
  int ExtractFont(std::string tabFile, std::string inputFile, std::string outDir);
}

#endif