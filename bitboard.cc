// bitboard.cc

#include "bitboard.h"

namespace chessy {

std::string Bitboard::ToString() {
  std::string res;
  for (int rank = 7; rank >= 0; --rank) {
    for (int file = 0; file < kRow; ++file) {
      res += (bits_ & Bitboard(rank, file).bits_) ? '1' : '0';
    }
    res += '\n';
  }
  return res;
}

std::ostream& operator<<(std::ostream& os, Bitboard board) {
  os << board.ToString();
  return os;
}

}  // namespace chessy
