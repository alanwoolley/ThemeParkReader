#include "tab/tab.hpp"
#include "gtest/gtest.h"
#include "util.hpp"

#include <iostream>

using namespace reader;
using namespace std;




TEST(Tab, ParseSpriteTab) {

  std::vector<char> test_tab_data = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x00, 0x30, 0x2C,
      0x9B, 0x08, 0x00, 0x00, 0x30, 0x2C,
      0x34, 0x11, 0x00, 0x00, 0x30, 0x2C};

  vectorwrapbuf<char> x(test_tab_data);

  std::istream t(&x);

  auto tab = ParseSpriteTab(t);
  int entry_count = tab.entries().size();


  ASSERT_EQ(3, entry_count);
  
  EXPECT_EQ(2, tab.entries()[0].dataoffset);
  EXPECT_EQ(48, tab.entries()[0].width);
  EXPECT_EQ(44, tab.entries()[0].height);

  EXPECT_EQ(2203, tab.entries()[1].dataoffset);
  EXPECT_EQ(48, tab.entries()[1].width);
  EXPECT_EQ(44, tab.entries()[1].height);

  EXPECT_EQ(4404, tab.entries()[2].dataoffset);
  EXPECT_EQ(48, tab.entries()[2].width);
  EXPECT_EQ(44, tab.entries()[2].height);
};

