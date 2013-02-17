// render.cc - displaying chessy with ANSI
// 2013.02.09

#include "render.h"

#include <iostream>
#include <sstream>

#include "board.h"
#include "move.h"
#include "term.h"

using std::cout;
using std::endl;
using std::string;

namespace chessy {

// Square scaling and centering factor.
const int kMid = render::kSquareCenter;
const int kSpan = render::kSquareSize;

void CursorToSquare(Square square) {
  int rank = square.rank();
  int file = square.file();
  int x = term::kBoardLeft + term::kBoardPad +
          (file * kSpan * 2);
  int y = term::kBoardPad + term::kBoardStart +
          (7-rank) * kSpan;
  cout << term::kX + term::i2s(y) + ";" + term::i2s(x) + "H";
  cout << term::kReset;
}

string CheckerColor(Square square, bool highlight) {
  string res = "";
  bool white = (square.rank() + square.file()) % 2;
  if (highlight) {
    res = white ? term::kWhiteSquareActive : term::kBlackSquareActive;
  } else {
    res = white ? term::kWhiteSquare : term::kBlackSquare;
  }
  return res;
}

void DrawSquare(string item) {
  for (int y = 0; y < kSpan; ++y) {
    for (int x = 0; x < kSpan; ++x) {
      cout << ((kMid == x && kMid == y) ? item : " ") << " ";
    }
    cout << term::kDown << term::kBackSquare;
  }
  cout << term::kReset;
}

namespace render {

// Highlighting squares so it's easier to see what just happened.
Square last_square_ = Square::kInvalid;
string last_glyph_ = " ";

void Status(string msg) {
  cout << term::kHideCursor
       << term::kStatusPosition << term::kClearEnd
       << term::kStatusColor;
  cout << msg;
  cout << term::kReset;
}

bool YesNoPrompt(string msg) {
  cout << term::kShowCursor;
  cout << msg << term::kPink << " [y/n]: " << term::kRed;
  string option;
  std::getline(std::cin, option);
  cout << term::kReset;
  return std::toupper(option[0]) == 'Y';
}

void ChessyMsg(string msg) {
  cout << term::kHideCursor << term::kRestoreCursor;
  cout << msg;
  cout << term::kSaveCursor;
}

void ChessyNewMsg(string msg) {
  cout << term::kClearAboveBoard
       << term::kChessyPosition
       << term::kChessy;
  ChessyMsg(msg);
}

void ChessyNewMsgMirror(string msg) {
  cout << term::kClearBelowBoard
       << term::kStatusPosition
       << term::kChessy;
  ChessyMsg(msg);
}

string HumanMovePrompt() {
  string input;
  cout << term::kInputPosition;
  cout << term::kOrange;
  cout << term::kShowCursor;
  cout << "Input move: " << term::kWhite;
  std::getline(std::cin, input);
  cout << term::kStatusPosition
       << term::kClearEnd;
  return input;
}

void UpdateBoard(const Board& board, const Bitmove& move) {
  // Should be drawn *after* the real board update.
  auto& source = move.source;
  auto& dest = move.dest;
  const Piece& tile = board.GetPiece(dest);
  string glyph = ((tile.color() == kWhite)
                  ? term::kWhitePiece
                  : term::kBlackPiece) + tile.GetGlyph();

  cout << term::kHideCursor;
  CursorToSquare(source);
  cout << CheckerColor(source, false);
  DrawSquare(" ");

  // Unhighlight previous square.
  if (" " != last_glyph_) {
    CursorToSquare(last_square_);
    cout << CheckerColor(last_square_, false);
    DrawSquare(last_glyph_);
  }

  CursorToSquare(dest);
  cout << CheckerColor(dest, true);
  DrawSquare(glyph);

  last_square_ = dest;
  last_glyph_ = glyph;
}

void Everything(const Board& board) {
  // clears and redraws all the things!
  cout << term::kClear << term::kBold
       << term::kBoardPosition
       << board
       << endl;
}

}  // namespace render

inline void RenderFileRow(std::ostream& os) {
  os << term::kGray;
  os << term::kBoardMargin << "  ";
  for (int file = 0; file < kRow; ++file) {
    for (int x = 0; x < kSpan; ++x) {
      if (x == kMid)
        os << static_cast<char>('A' + file) << " ";
      else
        os << "  ";
    }
  }
  os << endl;
}

void Board::Print(std::ostream& os, bool redraw) const {
  // TODO If |redraw|, then also re-render the squares and labels.
  // Otherwise, only update the piece chars

  // TODO: Ossified mode! (When a game is over, preserve the board but in a
  // different color)

  RenderFileRow(os);  // Top-side file letters
  cout << endl;

  // Print in reverse - rank 8 is top, 1 is bottom
  for (int rank = kRow - 1; rank >= 0; --rank) {
    // Rows within rows, (for enlarged squares)
    for (int i = 0; i < kSpan; ++i) {
      os << term::kReset;
      os << term::kBoardMargin;

      if (kMid == i) {  // Left-side rank numbers
        os << term::kGray << (rank + 1) << " ";
      } else {
        os << "  ";
      }

      for (int file = 0; file < kRow; ++file) {
        Square square(rank, file);
        const Piece& tile = squares_[square];
        os << CheckerColor(square, false);

        // Columns within columns.
        for (int j = 0 ; j < kSpan ; ++j)
          if (kMid == i && kMid == j) {
            os << ((tile.color() == kWhite)
                   ? term::kWhitePiece
                   : term::kBlackPiece)
               << tile.GetGlyph()
               << " ";
          } else {
            os << "  ";
          }
      }

      os << term::kReset;
      if (kMid == i) {  // Left-side rank numbers
        os << " " << term::kGray << rank + 1;
      }

      os << endl; // Next internal row
    }
  }
  cout << endl;
  RenderFileRow(os);  // Top-side file letters

  os << term::kReset << endl;
}

// string Board::StringAt(Square square) const {
//   Piece piece = PieceAt(square);
//   if (kEmpty == piece) {
//     return "Empty";
//   }
//   string name = kPieces[piece];
//   return ColorString(ColorAt(square)) + " " + name;
// }

// string Board::color_str() const {
//   return ColorString(color_);
// }

}  // namespace chessy
