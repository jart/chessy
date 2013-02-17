// bitboard.h

#ifndef CHESSY_BITBOARD_H_
#define CHESSY_BITBOARD_H_

#include <cstdint>
#include <ostream>
#include <string>

#include "square.h"

namespace chessy {

class Bitboard {
 public:
  constexpr Bitboard() : bits_(0) {}
  constexpr explicit Bitboard(uint64_t bits) : bits_(bits) {}
  constexpr Bitboard(int rank, int file)
      : bits_(1ull << (rank * 8) << file) {}
  constexpr explicit Bitboard(Square square)
      : Bitboard(square.rank(), square.file()) {}
  constexpr inline operator bool() const { return bits_ != 0; }
  constexpr inline operator int() const  { return bits_;      }
  constexpr inline uint64_t bits() const { return bits_;      }
  std::string ToString();

  inline Bitboard operator|(const Bitboard& other) const {
    return Bitboard(bits_ | other.bits_);
  }

  inline void operator|=(const Bitboard& other) {
    bits_ |= other.bits_;
  }

  inline Bitboard operator&(const Bitboard& other) const {
    return Bitboard(bits_ & other.bits_);
  }

  inline void operator&=(const Bitboard& other) {
    bits_ &= other.bits_;
  }

  inline Bitboard operator^(const Bitboard& other) const {
    return Bitboard(bits_ ^ other.bits_);
  }

  inline void operator^=(const Bitboard& other) {
    bits_ ^= other.bits_;
  }

 private:
  uint64_t bits_;
};

std::ostream& operator<<(std::ostream& os, Bitboard board);

}  // namespace chessy

#endif  // CHESSY_BITBOARD_H_
