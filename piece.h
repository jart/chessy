// piece.h - represents an individual tile on a chess board

#ifndef CHESSY_TILE_H_
#define CHESSY_TILE_H_

#include <cstdint>
#include <ostream>
#include <string>

namespace chessy {

enum Colors {
  kWhite = 0,
  kBlack = 1,
};

enum Pieces {
  kEmpty  = 0,
  kPawn   = 1,
  kKnight = 2,
  kBishop = 3,
  kRook   = 4,
  kQueen  = 5,
  kKing   = 6,
};

class Piece {
 public:
  constexpr Piece() : bits_(0) {}
  constexpr Piece(Colors color, Pieces piece)
      : bits_(piece | (color << 3)) {}
  constexpr inline operator int() const  { return bits_;                }
  constexpr inline operator bool() const { return !IsEmpty();           }
  constexpr inline uint8_t bits() const  { return bits_;                }
  constexpr inline Pieces piece() const  { return (Pieces)(bits_ & 7);  }
  constexpr inline Colors color() const  { return (Colors)(bits_ >> 3); }
  constexpr inline bool IsEmpty() const  { return !(bits_ & 7);         }
  int value() const;
  std::string Describe() const;
  const std::string& GetGlyph() const;
 private:
  uint8_t bits_;  // Uses three bits for piece, one bit for color.
};

const int kColors = 2;
const int kPieces = 7;
const int kCentipawn = 100;

inline Colors Toggle(Colors color) {
  return (color == kWhite) ? kBlack : kWhite;
}

const std::string& GetColorName(Colors color);
const std::string& GetPieceName(Pieces color);

std::ostream& operator<<(std::ostream& os, Piece piece);
std::ostream& operator<<(std::ostream& os, Colors color);
std::ostream& operator<<(std::ostream& os, Pieces piece);

}  // namespace chessy

#endif  // CHESSY_TILE_H_
