#include "palette/palette.hpp"
#include "util.hpp"
#include "gtest/gtest.h"

using namespace reader;
using namespace std;

TEST(Palette, Parse) {
  std::vector<char> test_palette = {
    0x00, 0x00, 0x00,
    0x01, 0x02, 0x03,
    0x11, 0x12, 0x13,
    0x21, 0x22, 0x23
  };

  vectorwrapbuf<char> x(test_palette);

  std::istream is(&x);

  auto pal = Palette::Parse(is);

  EXPECT_EQ(0x00, pal.colours[0].red);
  EXPECT_EQ(0x00, pal.colours[0].green);
  EXPECT_EQ(0x00, pal.colours[0].blue);

  EXPECT_EQ(0x01, pal.colours[1].red);
  EXPECT_EQ(0x02, pal.colours[1].green);
  EXPECT_EQ(0x03, pal.colours[1].blue);

  EXPECT_EQ(0x11, pal.colours[2].red);
  EXPECT_EQ(0x12, pal.colours[2].green);
  EXPECT_EQ(0x13, pal.colours[2].blue);

  EXPECT_EQ(0x21, pal.colours[3].red);
  EXPECT_EQ(0x22, pal.colours[3].green);
  EXPECT_EQ(0x23, pal.colours[3].blue);
}