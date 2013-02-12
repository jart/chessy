// render.h - board rendering helpers
// 2013.02.09

#ifndef CHESSY_RENDER_H_
#define CHESSY_RENDER_H_

#include <iostream>
#include <sstream>
#include "board.h"

namespace chessy {
namespace render {  // Main chessy rendering interface

  const int kSquareSize   = 3;  // in terms of console characters
  const int kSquareCenter = kSquareSize / 2; 

  void Status(string msg);
  bool YesNoPrompt(string msg);

  // ChessyMsg by itself appends messages, but requires
  // one of ChessyNewMsg() or ChessyNewMsgMirror() to have been called in the 
  // past. The second two functions clears either below or above the chess 
  // board, and "begin" a new chessy message streams
  void ChessyMsg(string msg);
  void ChessyNewMsg(string msg);  // Clears Chessy's text
  void ChessyNewMsgMirror(string msg);

  string HumanMovePrompt();
  // UpdateBoard should be called in most cases, rather than Board::Print()
  // because this redraws a mimimal number of console chars in response to a 
  // move update, while Board::Print draw *every* character over again.
  void UpdateBoard(Board* board, Move move);

  void Everything(Board* board);
}  // render

// TODO: make these stupid conversion thingies less dumb
string i2s(int x);
string x2s(int x);
  
namespace term {
  // ANSI escape sequences for console drawing shenanigans
  // 256 reference: http://www.pixelbeat.org/docs/terminal_colours/
  const string kX = "\x1b[";  // *the* escape character

  // --- Visibility ---
  const string kHideCursor    = kX + "?25l";
  const string kShowCursor    = kX + "?25h";
  const string kSaveCursor    = kX + "s";
  const string kRestoreCursor = kX + "u";

  const string kClear      = kX + "2J" + kX + "H";  // Clear ALL
  const string kClearEnd   = kX + "0J";   // only from cursor to end
  const string kClearStart = kX + "1J";   // only from cursor to start

  // --- Position/Offsets ---
  const int kBoardStart  = 12;  // Rows from the top
  const int kBoardLeft   = 4; 
  const int kBoardPad    = 2;   // For the Rank numbers
  const int kBoardHeight = kRow * render::kSquareSize;
  const int kBottom_ = kBoardStart + kBoardHeight + 7;

  // Cursor jumping
  const string kBoardPosition  = kX + i2s(kBoardStart) + ";1H";
  const string kInputPosition  = kX + i2s(kBottom_) + ";8H";
  const string kStatusPosition = kX + i2s(kBottom_ - 2) + ";4H";
  const string kChessyPosition = kX + "4;4H";
  const string kBoardMargin = string(kBoardLeft - 1, ' ');

  const string kDown = kX + "1B";
  const string kBack = kX + "1D";
  const string kBackSquare = kX + i2s(render::kSquareSize * 2) + "D";

  // --- Color ---
  const string kReset = kX + "0m";  // Reset in terms of color
  const string kBold = kX + "38;5;255;0;0m";
  const string kFG = kX + "38;5;";
  const string kBG = kX + "48;5;";

  // Foreground/text
  const string kWhite        = kX  + "37m";
  const string kBlack        = kX  + "30m";
  const string kGray         = kFG + "241m";
  const string kPurple       = kFG + "55m";
  const string kRed          = kFG + "196m";
  const string kPink         = kFG + "213m";
  const string kRose         = kFG + "212m";
  const string kOrange       = kFG + "203m";
  const string kYellow       = kFG + "226m";
  const string kGreen        = kFG + "46m";
  const string kBlue         = kFG + "14m";
  const string kNavy         = kFG + "61m";
  // Background
  const string kWhiteBg      = kX  + "47m";
  const string kGrayBg       = kBG + "238m";
  const string kBlackBg      = kBG + "234m";
  const string kLightGrayBg  = kBG + "242m";
  const string kLightBlackBg = kBG + "235m";
  const string kRedBg        = kBG + "196m";
  const string kPinkBg       = kBG + "182m";
  const string kOrangeBg     = kBG + "203m";
  const string kBlueBg       = kBG + "61m";
  const string kDawnBg       = kBG + "59m";
  const string kDuskBg       = kBG + "60m";


  // --- Application ---
  const string kWhitePiece = kPink;
  const string kWhiteSquare = kGrayBg;
  const string kWhiteSquareActive = kDawnBg;

  const string kBlackPiece = kBlue;
  const string kBlackSquare = kBlackBg;
  const string kBlackSquareActive = kDuskBg;

  const string kStatusColor  = kRed;
  const string kChessyPrompt = kRose + "Chessy: ";
  const string kChessyColor  = kWhite;
  const string kChessy       = kChessyPrompt + kChessyColor + kSaveCursor;

  const string kClearAboveBoard = kBoardPosition  + kClearStart;
  const string kClearBelowBoard = kStatusPosition + kClearEnd;
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

  string ColorString(Color color);
}  // chessy

#endif  // CHESSY_RENDER_H_
