#include "tab/tab.hpp"
#include "gtest/gtest.h"

#include <iostream>

using namespace reader;
using namespace std;

namespace
{

template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT> {

public:
  vectorwrapbuf(std::vector<CharT> vec)  {
    _buf = vec;
    this->setg(&_buf[0], &_buf[0], &_buf[0] + _buf.size());
  }

protected:

  typename std::basic_streambuf<CharT, TraitsT>::pos_type seekoff(typename std::basic_streambuf<CharT, TraitsT>::off_type off,
                   std::ios_base::seekdir dir,
                   std::ios_base::openmode which = std::ios_base::in)
  {
    if (dir == std::ios_base::cur)
      this->gbump(off);
    else if (dir == std::ios_base::end)
      this->setg(this->eback(), this->egptr() + off, this->egptr());
    else if (dir == std::ios_base::beg)
      this->setg(this->eback(), this->eback() + off, this->egptr());
    return this->gptr() - this->eback();

  }

private:
  std::vector<CharT> _buf;
};

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

}