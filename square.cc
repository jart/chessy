// square.h
// 2013.02.08

#include "square.h"

#include <cassert>

#include "term.h"

using std::string;

namespace chessy {

bool Valid(Square square) {
  return !(square & kInvalidMask);
}

Square Getx88(Offset rank, Offset file) {
  return kRank * rank + file;
}

Square Getx88(char f, char r) {
  Offset file = tolower(f) - 'a';
  if (file < 0 || file >= kRow)
    return kInvalidSquare;
  Offset rank = r - '1';
  if (rank < 0 || rank >= kRow)
    return kInvalidSquare;
  return Getx88(rank, file);
}

Square Getx88(int index) {
  if (index < 0 || index >= kTotal)
    return kInvalidSquare;
  return kRank * (index >> 3) + index % kRow;
}

int Index(Square square) {
  assert(Valid(square));
  return square % kRow + Rank(square) * kRow;
}

Offset Rank(Square square) {
  return square >> kRankShift;  // 4th through 7th bit
}

Offset File(Square square) {
  return square & kFileMask;    // First 3 bits
}

string PrintSquare(const Square& square) {
  if (!Valid(square)) {
    return "NA(" + term::x2s(square) + ")";
  }
  string res;
  res += 'a' + static_cast<int>(File(square));
  res += '1' + static_cast<int>(Rank(square));
  return res;
}

std::ostream& operator<<(std::ostream& os, const Square& square) {
  os << PrintSquare(square);
  return os;
}

}  // namespace chessy
