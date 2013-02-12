// chessy - a chess engine by justine and serene
// 2013.02.03

#ifndef CHESSY_H_
#define CHESSY_H_

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <string>

using std::array;
using std::string;
using std::bitset;


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
    kQueen   =  4,
    kKing    = 5,
    kNoPiece = 6,
  };

  const int kColors = 2;
  const int kPieceTypes = 6;
  const int kMaxPieces = 16;

  typedef int8_t Bitx88;  // 0x88 bit masks
  typedef Bitx88 Square;  // Singular square (Assumes 0x88 representation)
  typedef int8_t Offset;  // Regular signed integer offsets

  // Returns the color-modified Piece-type index
  int PieceIndex(Color color, Piece piece);

  enum GameMode {
    kMirror = 0,  // Bot plays its self
    kHuman = 1,  // Bot plays YOU!
  };

  enum GameState {
    kNone = 0,
    kPlaying = 1,
    kPaused = 2,
  };

  extern GameMode g_mode;
  extern GameState g_state;

  extern int g_dbg;  // Debug level from 0-2

  void GameLoop();
  Color Toggle(Color color);

}  // chessy

#endif  // CHESSY_H_
