// render.h - board rendering helpers
// 2013.02.09

#ifndef CHESSY_RENDER_H_
#define CHESSY_RENDER_H_

#include <array>
#include <iostream>

#include "board.h"
#include "move.h"

namespace chessy {

  namespace terminal {
    // Terminal coloring
    const string kReset = "\x1b[0m";
    const string kGray = "\x1b[38;5;241m";
    const string kBlack = "\x1b[30m";
    const string kWhiteBg = "\x1b[47m";
    const string kGrayBg = "\x1b[48;5;246m";
  }

// #define UNICODE
  // Some terminals are happy, and some are SAD!
  // Disable UNICODE to print in pure ascii rather than use the specal chess 
  // characters.

#ifdef UNICODE
  const std::array<string, kPieceTypes * kColors> kPieceString = {{
    u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔",
    u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚",
  }};

#else
  const std::array<string, kPieceTypes * kColors> kPieceString = {{
     "P", "N", "B", "R", "Q", "K",
     "p", "n", "b", "r", "q", "k",
  }};
#endif

  namespace names {

    // Names for all the things!
    const array<string, kMoveTypes> kMoveTypes = {{
      "Invalid",
      "Regular",
      "Capture",
      "Castle",
      "Queen-side Castle",
      "Check",
      "NullMove",
      "Tentative",
    }};

    const array<string, kPieceTypes> kPieces = {{
      "Pawn",
      "Knight",
      "Bishop",
      "Rook",
      "Queen",
      "King",
    }};

  }

  std::ostream& operator<<(std::ostream& os, const Board& board);
  std::ostream& operator<<(std::ostream& os, const Square& square);
  std::ostream& operator<<(std::ostream& os, const MoveType& type);
  std::ostream& operator<<(std::ostream& os, const Move& move);
  std::ostream& operator<<(std::ostream& os, const Piece& piece);

  void DepthCout(int depth);
  string ColorString(Color color);


}  // chessy

#endif  // CHESSY_RENDER_H_
