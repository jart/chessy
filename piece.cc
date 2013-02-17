// piece.cc - represents an individual tile on a chess board

#include "piece.h"

namespace chessy {

static const int kPieceValues[] = {
  [kEmpty]  =   0 * kCentipawn,
  [kPawn]   =   1 * kCentipawn,
  [kKnight] =   3 * kCentipawn,
  [kBishop] =   3 * kCentipawn,
  [kRook]   =   5 * kCentipawn,
  [kQueen]  =   9 * kCentipawn,
  [kKing]   = 666 * kCentipawn,
};

static const std::string kColorNames[] = {
  "White",
  "Black",
};

static const std::string kPieceNames[] = {
  "Empty",
  "Pawn",
  "Knight",
  "Bishop",
  "Rook",
  "Queen",
  "King",
};

static const std::string kGlyphs[] = {
#ifdef UNICODE
  u8" ", u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔",
  u8" ", u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚",
#else
  " ", "P", "N", "B", "R", "Q", "K",
  " ", "p", "n", "b", "r", "q", "k",
#endif
};

int Piece::value() const {
  return kPieceValues[piece()];
}

std::string Piece::Describe() const {
  if (IsEmpty()) {
    return kPieceNames[kEmpty];
  } else {
    return GetColorName(color()) + " " + GetPieceName(piece());
  }
}

const std::string& Piece::GetGlyph() const {
  return kGlyphs[color() * kPieces + piece()];
}

const std::string& GetColorName(Colors color) {
  return kColorNames[color];
}

const std::string& GetPieceName(Pieces piece) {
  return kPieceNames[piece];
}

std::ostream& operator<<(std::ostream& os, Piece piece) {
  os << piece.Describe();
  return os;
}

std::ostream& operator<<(std::ostream& os, Pieces piece) {
  os << GetPieceName(piece);
  return os;
}

std::ostream& operator<<(std::ostream& os, Colors color) {
  os << GetColorName(color);
  return os;
}

}  // namespace chessy
