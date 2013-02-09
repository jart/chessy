// render.h - board rendering helpers
// 2013.02.09

#ifndef CHESSY_RENDER_H_
#define CHESSY_RENDER_H_

#include <array>
#include <iostream>

#include "board.h"

namespace chessy {

// Terminal coloring
const string kColorReset = "\x1b[0m";
const string kColorGray = "\x1b[38;5;241m";
const string kColorBlack = "\x1b[30m";
const string kColorWhiteBg = "\x1b[47m";
const string kColorGrayBg = "\x1b[48;5;246m";

// Some terminals are happy, and some are SAD!
#define UNICODE
#ifdef UNICODE
const std::array<string, kPieceTypes * kColors> kPieceString = {{
  u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔",
  u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚",
}};
#else
const std::array<string, kPieceTypes * kColors> kPieceString = {{
   "P", "N", "B", "R", "Q", "K",
   "P", "N", "B", "R", "Q", "K",
}};
#endif

std::ostream& operator<<(std::ostream& os, const Board& board);
std::ostream& operator<<(std::ostream& os, const Square& square);
std::ostream& operator<<(std::ostream& os, const Move& move);

void DepthCout(int depth);
string ColorString(Color color);


}  // chessy

#endif  // CHESSY_RENDER_H_
