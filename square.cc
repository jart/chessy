// square.cc - 0x88 board indexing.

#include "square.h"
#include <cctype>
#include "term.h"

namespace chessy {

const Square Square::kUp      = Square( 1,  0);
const Square Square::kDown    = Square(-1,  0);
const Square Square::kLeft    = Square( 0, -1);
const Square Square::kRight   = Square( 0,  1);
const Square Square::kInvalid = Square(0x88);

Square Square::Parse(std::string str) {
  if (str.size() != 2)
    return kInvalid;
  int file = std::tolower(str[0]) - 'a';
  if (file < 0 || file >= kRow)
    return kInvalid;
  int rank = str[1] - '1';
  if (rank < 0 || rank >= kRow)
    return kInvalid;
  return Square(rank, file);
}

std::string Square::ToString() const {
  if (!Valid()) {
    return "NA(" + term::x2s(x88_) + ")";
  }
  std::string res;
  res += 'a' + File();
  res += '1' + Rank();
  return res;
}

std::ostream& operator<<(std::ostream& os, Square square) {
  os << square.ToString();
  return os;
}

}  // namespace chessy
