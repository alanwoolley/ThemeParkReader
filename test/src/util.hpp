#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <streambuf>
#include <vector>

template<typename CharT, typename TraitsT = std::char_traits<CharT>>
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



#endif