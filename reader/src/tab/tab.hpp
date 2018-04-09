#ifndef TAB_H_
#define TAB_H_

#include <vector>
#include <string>

namespace reader {

template <class T>
class Tab {
  public:
		Tab(std::vector<T>& e) : entries_(e) {}
    const std::vector<T> & entries() { return entries_; };
  private:
    const std::vector<T> entries_;
};

typedef struct {
	uint32_t dataoffset;
	uint8_t width;
	uint8_t height;
} SpriteFile;

typedef struct {
	char filename[13];
	uint32_t dataoffset;
	uint32_t datasize;
} MusicFile;

typedef struct {
	uint32_t dataoffset;
	uint8_t width;
	uint8_t height;
} FontCharacter;

using SpriteTab = Tab<SpriteFile>;
using MusicTab = Tab<MusicFile>;
using FontTab = Tab<FontCharacter>;

SpriteTab ParseSpriteTab(std::istream& tabFileIn);
MusicTab ParseMusicTab(std::istream& tabFileIn);
FontTab ParseFontTab(std::istream& tabFileIn);
}

#endif