// square.h
// 2013.02.08

#ifndef CHESSY_SQUARE_H_
#define CHESSY_SQUARE_H_

#include <cstdint>
#include <array>
#include <ostream>
#include <string>

namespace chessy {

typedef int8_t Offset;  // Regular signed integer offsets.
typedef int8_t Square;  // Singular square (Assumes 0x88 representation)

const int kTotal            = 0x40;  // 64
const Offset kRank          = 0x10;  // 16
const Offset kFile          = 0x01;
const Offset kRankShift     = 4;
const Square kFileMask      = 0x07;
const Square kInvalidSquare = 0x80;  // Default "invalid" square.
const Square kInvalidMask   = 0x88;
const Offset kRow           = 8;

// Conversions between Rank, File numbers, Indices, and 0x88.
Offset Rank(Square square);
Offset File(Square square);
Square Getx88(Offset rank, Offset file);
Square Getx88(char f, char r);
Square Getx88(int index);
int Index(Square square);

bool Valid(Square square);

std::string PrintSquare(const Square& square);

std::ostream& operator<<(std::ostream& os, const Square& square);

namespace delta {

const Offset kU  = kRank;
const Offset kD  = -kU;
const Offset kR  = kFile;
const Offset kL  = -kR;
const Offset kUR = kU + kR;
const Offset kUL = kU + kL;
const Offset kDR = kD + kR;
const Offset kDL = kD + kL;

const std::array<Offset, 4> kOrthogonal = {{
  kU, kD, kR, kL,
}};

const std::array<Offset, 4> kDiagonal = {{
  kUR, kUL, kDR, kDL,
}};

const std::array<Offset, 8> kOmnigonal = {{
  kU, kD, kR, kL, kUR, kUL, kDR, kDL,
}};

// The 8 'L' shapes.
const std::array<Offset, 8> kKnight = {{
  kU + kUR, kU + kUL,
  kD + kDR, kD + kDL,
  kR + kUR, kR + kDR,
  kL + kUL, kL + kDL,
}};

}  // namespace delta

}  // namespace chessy

#endif  // CHESSY_SQUARE_H_
