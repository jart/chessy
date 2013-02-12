// render.cc - displaying chessy with ANSI
// 2013.02.09

#include "render.h"

using std::cout;
using std::endl;

namespace chessy {

  // Square scaling and centering factor
  static const int kSpan_ = render::kSquareSize;
  static const int kMid_  = render::kSquareCenter;

  // TODO: Make this not suck 
  string i2s(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
  }
  string x2s(int x) {
    std::stringstream ss;
    ss << std::hex << x;
    return ss.str();
  }

  void CursorToSquare(Square square) {

    Offset rank = square::Rank(square);
    Offset file = square::File(square);
    int x = term::kBoardLeft + term::kBoardPad +
            (file * kSpan_ * 2);
    int y = term::kBoardPad + term::kBoardStart +
            (7-rank) * kSpan_;
    cout << term::kX + i2s(y) + ";" + i2s(x) + "H";
    cout << term::kReset;
  }

  string CheckerColor(Square square, bool highlight) {
    // checkered background pattern
    string res = "";
    bool white = (square::Rank(square) + square::File(square)) % 2;

    // highlighting
    if (highlight)
      res = white ? term::kWhiteSquareActive : term::kBlackSquareActive;
    else
      res = white ? term::kWhiteSquare : term::kBlackSquare;
  
    return res;
  }

  void DrawSquare(string item) {
    for (int y = 0 ; y < kSpan_ ; ++y) {
      for (int x = 0 ; x < kSpan_ ; ++x) {
        cout << ((kMid_ == x && kMid_ == y) ? item : " ") << " ";
      }
      cout << term::kDown << term::kBackSquare;
    }
    cout << term::kReset;
  }

// -------------- Main ANSI rendering --------------
namespace render {

  // highlighting squares so it's easier to see what just happened
  Square last_square_ = square::kInvalid;
  string last_piece_ = " ";

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

  void UpdateBoard(Board* board, Move move) {
    // Should be drawn *after* the real board update
    auto& source = move.source;
    auto& dest = move.dest;
    Color color = board->ColorAt(source);
    string piece = color == kWhite ? term::kWhitePiece : term::kBlackPiece;
    piece += kPieceString[PieceIndex(
        color,
        board->PieceAt(dest))];

    cout << term::kHideCursor;
    CursorToSquare(source);
    cout << CheckerColor(source, false);
    DrawSquare(" ");

    // Unhighlight previous square
    if (" " != last_piece_) {
      CursorToSquare(last_square_);
      cout << CheckerColor(last_square_, false);
      DrawSquare(last_piece_);
    }

    CursorToSquare(dest);
    cout << CheckerColor(dest, true);
    DrawSquare(piece);

    last_square_ = dest;
    last_piece_ = piece;
  }

  void Everything(Board* board) {
    // clears and redraws all the things!
    cout << term::kClear << term::kBold;
    cout << *board << endl;
  }
}  // render


  inline void RenderFileRow(std::ostream& os) {
    os << term::kGray;
    os << term::kBoardMargin << "  ";
    for (int file = 0 ; file < kRow ; ++file) {
      for (int x = 0 ; x < kSpan_ ; ++x) {
        if (x == kMid_)
          os << static_cast<char>('A' + file) << " ";
        else
          os << "  ";
      }
    }
    os << endl;
  }

  void Board::Print(std::ostream& os, bool redraw) const {
    array<string, square::kTotal> sboard;

    // Fill piece data
    sboard.fill(" ");
    for (const auto& state : square_table_) {
      if (!state.empty) {
        sboard[state.index] = state.color == kWhite ? term::kWhitePiece : term::kBlackPiece;
        sboard[state.index] += kPieceString[PieceIndex(state.color, state.piece)];
      }
    }

    // TODO If |redraw|, then also re-render the squares and labels.
    // Otherwise, only update the piece chars
    
    // TODO: Ossified mode! (When a game is over, preserve the board but in a 
    // different color)

    os << term::kBoardPosition;
    RenderFileRow(os);  // Top-side file letters
    cout << endl;

    // Print in reverse - rank 8 is top, 1 is bottom
    for (Offset rank = kRow - 1; rank >= 0; --rank) {
      // Rows within rows, (for enlarged squares)
      for (int i = 0 ; i < kSpan_ ; ++i) {

        os << term::kReset;
        os << term::kBoardMargin;

        if (kMid_ == i)  // Left-side rank numbers
          os << term::kGray << (rank + 1) << " "; 
        else
          os << "  ";

        for (Offset file = 0; file < kRow; ++file) {
          Square square = square::Getx88(rank, file);
          os << CheckerColor(square, false);

          // Columns within columns
          for (int j = 0 ; j < kSpan_ ; ++j)
            if (kMid_ == i && kMid_ == j)
              os << sboard[rank * kRow + file] << " "; // Draw the actual piece
            else
              os << "  ";
        }

        os << term::kReset;
        if (kMid_ == i)  // Left-side rank numbers
          os << " " << term::kGray << rank + 1; 

        os << endl; // Next internal row
      }
    }
    cout << endl;
    RenderFileRow(os);  // Top-side file letters

    os << term::kReset << endl;
  }

  string PrintSquare(const Square& square) {
    if (!square::Valid(square)) {
      return "NA(" + x2s(square) + ")";
    }
    string res;
    res += 'a' + (int)square::File(square);
    res += '1' + (int)square::Rank(square);
    return res;
  }
  string PrintMove(const Move& move) {
    string res = "";
    res += PrintSquare(move.source);
    res += "->";
    res += PrintSquare(move.dest);
    res += " [" + names::kMoveTypes[move.type] + "]";
    return res;
  }


  // cout << board
  std::ostream& operator<<(std::ostream& os, const Board& board) {
    board.Print(os, true);
    return os;
  }
  // cout << piece
  std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    os << names::kPieces[piece];
    return os;
  }
  // cout << square
  std::ostream& operator<<(std::ostream& os, const Square& square) {
    os << PrintSquare(square);
    return os;
  }
  // cout << movetype
  std::ostream& operator<<(std::ostream& os, const MoveType& type) {
    os << names::kMoveTypes[type];
    return os;
  }
  // cout << move
  std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << PrintMove(move);
    return os;
  }

  // misc.
  inline string ColorString(Color color) {
    return (kWhite == color) ? "White" : "Black";
  }
  string Board::StringAt(Square square) const {
    Piece piece = PieceAt(square);
    if (kNoPiece == piece) {
      return "Empty";
    }
    string name = names::kPieces[piece];
    return ColorString(ColorAt(square)) + " " + name;
  }

  string Board::color_str() const {
    return ColorString(color_);
  }
}  // chessy
