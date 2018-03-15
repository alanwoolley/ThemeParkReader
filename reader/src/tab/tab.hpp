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

using SpriteTab = Tab<SpriteFile>;
using MusicTab = Tab<MusicFile>;

SpriteTab ParseSpriteTab(std::istream& tabFileIn);
MusicTab ParseMusicTab(std::istream& tabFileIn);
SpriteTab ParseTest(std::istream& tabIn);
}
