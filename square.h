// square.h - 0x88 board indexing.

#ifndef CHESSY_SQUARE_H_
#define CHESSY_SQUARE_H_

#include <ostream>
#include <string>

namespace chessy {

static const int kRow = 8;

// Square implements the 0x88 optimization for board indexing. Square can be
// used to store the coordinates of a piece of the delta for a move. This data
// structure consists of a signed byte that stores the file (x-axis) in bits
// 1-3, the rank (y-axis) in bits 5-7, and bits 4 and 8 are used to detect
// overflow. The eponymous constant 0x88 is a mask of bits 4 and 8 which can
// quickly check for invalid positions.
class Square {
 public:
  static const Square kUp;
  static const Square kDown;
  static const Square kLeft;
  static const Square kRight;
  static const Square kInvalid;

  constexpr Square() : x88_(0) {}
  constexpr Square(int rank, int file) : x88_(rank * 0x10 + file) {}
  constexpr explicit Square(int8_t x88) : x88_(x88) {}
  Square(std::string str);
  Square(const char* str) : Square(std::string(str)) {}

  constexpr inline operator int() const { return x88_;           }
  constexpr inline int8_t x88() const   { return x88_;           }
  constexpr inline int rank() const     { return x88_ >> 4;      }
  constexpr inline int file() const     { return x88_ & 0x07;    }
  constexpr inline bool IsValid() const { return !(x88_ & 0x88); }

  constexpr inline bool operator==(Square other) const {
    return (x88_ == other.x88_);
  }

  constexpr inline Square operator+(Square other) const {
    return Square(x88_ + other.x88_);
  }

  constexpr inline Square operator-(Square other) const {
    return Square(x88_ - other.x88_);
  }

  std::string ToString() const;
  static Square Parse(std::string str);

 private:
  int8_t x88_;
};

std::ostream& operator<<(std::ostream& os, Square square);

}  // namespace chessy

#endif  // CHESSY_SQUARE_H_
