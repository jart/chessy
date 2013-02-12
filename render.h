// render.h - board rendering helpers
// 2013.02.09

#ifndef CHESSY_RENDER_H_
#define CHESSY_RENDER_H_

#include <array>
#include <iostream>
#include <sstream>

#include "board.h"
#include "move.h"

namespace chessy {

  namespace render {  // Main chessy rendering interface
    extern Square last_square;
    extern string last_piece;

    void ChessyMsg(string msg);
    void ChessyNewMsg(string msg);  // Clears Chessy's text
    void ChessyNewMsgMirror(string msg);
    void Status(string msg);

    string HumanPrompt();
    bool YesNoPrompt(string msg);

    void UpdateBoard(Board* board, Move move);
    void Everything(Board* board);
  }  // render

  string i2s(int x);

  namespace term {
    const string kX = "\x1b[";  // Escape character!

    // Terminal coloring
    const string kReset = "\x1b[0m";  // Reset in terms of color
    const string kBold = "\x1b[38;5;255;0;0m";

    const int kSquareSize   = 3;      // How BIG the chess board renders!
    const int kSquareCenter = kSquareSize / 2;

    const int kBoardStart  = 12;
    const int kBoardLeft   = 4;
    const int kBoardPad    = 2;
    const int kBoardHeight = kRow * kSquareSize;
    const int kBottom_ = kBoardStart + kBoardHeight + 7;

    // FG Colors
    const string kWhite = kX + "37m";
    const string kGray = "\x1b[38;5;241m";
    const string kBlack = "\x1b[30m";
    // const string kYellow = "\x1b[38;5;230m";
    const string kYellow = "\x1b[38;5;226m";
    const string kPink = "\x1b[38;5;213m";
    const string kRose = "\x1b[38;5;212m";
    const string kOrange = "\x1b[38;5;203m";
    const string kRed = "\x1b[38;5;196m";
    const string kGreen = "\x1b[38;5;46m";
    const string kBlue= "\x1b[38;5;14m";
///    const string kPurple= "\x1b[38;5;55m";
    const string kNavy= "\x1b[38;5;61m";
    const string kPurple= "\x1b[38;5;55m";

    // BG Colors
    const string kWhiteBg = "\x1b[47m";
    const string kGrayBg = "\x1b[48;5;238m";
    const string kLightGrayBg = "\x1b[48;5;242m";
    const string kBlackBg = "\x1b[48;5;234m";
    const string kLightBlackBg = "\x1b[48;5;235m";
    const string kOrangeBg = "\x1b[48;5;203m";
    const string kRedBg = "\x1b[48;5;196m";
    const string kBlueBg = "\x1b[48;5;61m";
    const string kDawnBg = "\x1b[48;5;59m";
    const string kDuskBg = "\x1b[48;5;60m";
    const string kPinkBg = "\x1b[48;5;182m";

    // Cursor hiding
    const string kHideCursor = kX + "?25l";
    const string kShowCursor = kX + "?25h";
    const string kSaveCursor = kX + "s";
    const string kRestoreCursor = kX + "u";

    const string kClear = "\x1b[2J \x1b[H";  // Clears entire screen
    const string kClearEnd = "\x1b[0J";      // Clears only from cursor to end
    const string kClearStart = "\x1b[1J";    // Clears only from cursor to start
    const string kDown = "\x1b[1B";
    const string kBack = "\x1b[1D";
    const string kBackSquare = kX + i2s(kSquareSize * 2) + "D";

    // Terminal positioning
    const string kBoardPosition = "\x1b[" + i2s(kBoardStart) + ";1H";
    const string kInputPosition = "\x1b[" + i2s(kBottom_) + ";8H";
    const string kStatusPosition = "\x1b[" + i2s(kBottom_ - 2) + ";4H";
    const string kChessyPosition = "\x1b[4;4H";
    const string kBoardMargin = string(kBoardLeft - 1, ' ');

    // Actual color settings
    const string kWhitePiece = kPink;
    const string kWhiteSquare = kGrayBg;
    const string kWhiteSquareActive = kDawnBg;
    const string kBlackPiece = kBlue;
    const string kBlackSquare = kBlackBg;
    const string kBlackSquareActive = kDuskBg;
    const string kChessPiece = kBlue;

    const string kStatusColor = kRed;
  }


// #define UNICODE
  // Some terms are happy, and some are SAD!
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

  string PrintSquare(const Square& square);
  string PrintMove(const Move& move);

  std::ostream& operator<<(std::ostream& os, const Board& board);
  std::ostream& operator<<(std::ostream& os, const Square& square);
  std::ostream& operator<<(std::ostream& os, const MoveType& type);
  std::ostream& operator<<(std::ostream& os, const Move& move);
  std::ostream& operator<<(std::ostream& os, const Piece& piece);

  void DepthCout(int depth);
  string ColorString(Color color);

  string i2s(int x);

}  // chessy

#endif  // CHESSY_RENDER_H_
