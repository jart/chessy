// chessy - a chess engine by justine and serene
// 2013.02.03

#ifndef CHESSY_H_
#define CHESSY_H_

#include <string>

namespace chessy {

enum Color {
  kWhite = 0,
  kBlack = 1,
};

enum Piece {
  kPawn    = 0,
  kKnight  = 1,
  kBishop  = 2,
  kRook    = 3,
  kQueen   = 4,
  kKing    = 5,
  kNoPiece = 6,
};

enum GameMode {
  kMirror = 0,  // Bot plays itself.
  kHuman = 1,  // Bot plays YOU!
};

enum GameState {
  kNone = 0,
  kPlaying = 1,
  kPaused = 2,
};

const int kColors = 2;
const int kPieceTypes = 6;
const int kMaxPieces = 16;

// Play the game.
void GameLoop();

// Stop the game loop (usually from a signal handler).
void EndGame();

// Returns the color-modified Piece-type index.
inline int PieceIndex(Color color, Piece piece) {
  return piece + (color == kWhite? 0 : kPieceTypes);
}

inline Color Toggle(Color color) {
  return (color == kWhite) ? kBlack : kWhite;
}

inline std::string ColorString(Color color) {
  return (color == kWhite) ? "White" : "Black";
}

}  // namespace chessy

#endif  // CHESSY_H_
