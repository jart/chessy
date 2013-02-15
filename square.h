// square.h - 0x88 board indexing.

#ifndef CHESSY_SQUARE_H_
#define CHESSY_SQUARE_H_

#include <ostream>
#include <string>

namespace chessy {

static const int kRow = 8;

// Square implements the 0x88 optimization for board indexing. This data
// structure consists of a signed byte that stores the file (x-axis) in bytes
// 1-3, the rank (y-axis) in bytes 5-7, and uses bytes 4 and 8 to detect
// overflow so we know if a position is invalid.
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

  constexpr inline int8_t x88() const   { return x88_;                   }
  constexpr inline int Rank() const     { return x88_ >> 4;              }
  constexpr inline int File() const     { return x88_ & 0x07;            }
  constexpr inline size_t Index() const { return Rank() * kRow + File(); }
  constexpr inline bool Valid() const   { return !(x88_ & 0x88);         }

  static constexpr inline Square FromIndex(int index) {
    return Square(index / kRow, index % kRow);
  }

  constexpr inline bool operator==(Square other) const {
    return (x88_ == other.x88_);
  }

  constexpr inline Square operator+(Square other) const {
    return Square(x88_ + other.x88_);
  }

  constexpr inline Square operator-(Square other) const {
    return Square(x88_ - other.x88_);
  }

  inline void operator+=(Square other) {
    x88_ += other.x88_;
  }

  inline void operator-=(Square other) {
    x88_ -= other.x88_;
  }

  std::string ToString() const;
  static Square Parse(std::string str);

 private:
  int8_t x88_;
};

std::ostream& operator<<(std::ostream& os, Square square);

}  // namespace chessy

#endif  // CHESSY_SQUARE_H_
