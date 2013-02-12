// 2013.02.09

#include <algorithm>

#include "chessy.h"
#include "board.h"
#include "move.h"
#include "bot.h"
#include "render.h"

using std::cout;
using std::endl;

// using namespace chessy::term;

namespace chessy {
 
  void CleanString(string& str) {
/*    for (char c : str) {
      if (!(c >= '0' && c<= 'z'))
        str.remove()
    }
    in.erase(std::remove(in.begin(), in.end() ,
    */
  }

  string i2s(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
  }

  void CursorToSquare(Square square) {

    Offset rank = square::Rank(square);
    Offset file = square::File(square);
    int x = term::kBoardLeft + term::kBoardPad +
            (file * term::kSquareSize * 2);
    int y = term::kBoardPad + term::kBoardStart +
            (7-rank) * term::kSquareSize;
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

    cout << term::kChessPiece; // Chess piece color

    for (int y = 0 ; y < term::kSquareSize ; ++y) {
      for (int x = 0 ; x < term::kSquareSize ; ++x) {
        if (term::kSquareCenter == x &&
            term::kSquareCenter == y) {
          cout << item << " ";
        } else {
          cout << "  ";
        }
      }
      cout << term::kDown << term::kBackSquare;
    }
    cout << term::kReset;
  }

  namespace render {
    bool fresh_board = true;
    const string kBlanks = "                                                ";
    Square last_square = square::kInvalid;
    string last_piece = " ";

    void Everything(Board* board) {

      // Clear console, redraw
      cout << term::kClear;
      cout << term::kBold;

      ChessyNewMsg(g_last_message);

      cout << endl << endl << endl;
      cout << *board << endl;
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
      if (" " != last_piece) {
        CursorToSquare(last_square);
        cout << CheckerColor(last_square, false);
        DrawSquare(last_piece);
      }

      CursorToSquare(dest);
      cout << CheckerColor(dest, true);
      DrawSquare(piece);

      last_square = dest;
      last_piece = piece;
    }

    string HumanPrompt() {
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

    bool YesNoPrompt(string msg) {
      cout << term::kShowCursor;
      cout << msg << term::kPink << " [y/n]: " << term::kRed;
      string option;
      std::getline(std::cin, option);
      return std::toupper(option[0]) == 'Y';
    }

    void ChessyNewMsg(string msg) {
      cout << term::kBoardPosition << term::kClearStart;

      cout << term::kChessyPosition 
           << term::kRose << "Chessy: "
           << term::kWhite;
      cout << term::kSaveCursor;
      ChessyMsg(msg);
    }
    void ChessyNewMsgMirror(string msg) {
      // When chessy plays against herself, put a mirror log below!
      cout << term::kStatusPosition << term::kClearEnd;

      cout << term::kStatusPosition 
           << term::kRose << "Chessy: "
           << term::kWhite;
      cout << term::kSaveCursor;
      ChessyMsg(msg);
    }
    void ChessyMsg(string msg) {
      cout << term::kHideCursor << term::kRestoreCursor; 
      cout << msg;
      cout << term::kSaveCursor; 
    }

    void Status(string msg) {
      cout << term::kStatusPosition << term::kClearEnd
           << term::kStatusColor;
      cout << msg;
      cout << term::kReset;
    }
  }

  inline void RenderFileRow(std::ostream& os) {
    os << term::kGray;
    os << term::kBoardMargin << "  ";
    for (int file = 0 ; file < kRow ; ++file) {
      for (int x = 0 ; x < term::kSquareSize ; ++x) {
        if (x == term::kSquareCenter)
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

    os << term::kBoardPosition;
    RenderFileRow(os);  // Top-side file letters
    cout << endl;
    // Print in reverse - rank 8 is top, 1 is bottom
    for (Offset rank = kRow - 1; rank >= 0; --rank) {
      // Rows within rows, (for enlarged squares)
      for (int i = 0 ; i < term::kSquareSize ; ++i) {

        os << term::kReset;
        os << term::kBoardMargin;

        if (term::kSquareCenter == i)  // Left-side rank numbers
          os << term::kGray << (rank + 1) << " "; 
        else
          os << "  ";

        for (Offset file = 0; file < kRow; ++file) {
          Square square = square::Getx88(rank, file);
          os << CheckerColor(square, false);

          // Columns within columns
          for (int j = 0 ; j < term::kSquareSize ; ++j)
            if (term::kSquareCenter == i && term::kSquareCenter == j)
              os << sboard[rank * kRow + file] << " "; // Draw the actual piece
            else
              os << "  ";
        }

        os << term::kReset;
        if (term::kSquareCenter == i)  // Left-side rank numbers
          os << " " << term::kGray << rank + 1; 

        os << endl; // Next internal row
      }
    }
    cout << endl;
    RenderFileRow(os);  // Top-side file letters

    os << term::kReset << endl;
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
  string PrintSquare(const Square& square) {
    string res;
    if (!square::Valid(square)) {
      res += "NA(";
      res += square;
      res += ")";
//      sprintf(res&, "NA(%02x)", square);
      return res;
    }
    res += 'a' + (int)square::File(square);
    res += '1' + (int)square::Rank(square);
    return res;
  }
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
  string PrintMove(const Move& move) {
    string res = "";
    res += PrintSquare(move.source);
    res += "->";
    res += PrintSquare(move.dest);
    res += " [" + names::kMoveTypes[move.type] + "]";
    return res;
  }
  std::ostream& operator<<(std::ostream& os, const Move& move) {
    /*
    if (g_dbg > 0) {
      os << "[" << move.type << "";
      if (kAttack == move.type) {
        os << " " << names::kPieces[move.captured];
      }
      os << "] ";
    } */
    os << PrintMove(move);
    return os;
  }

  inline string ColorString(Color color) {
    return (kWhite == color) ? "White" : "Black";
  }

  void DepthCout(int depth) {
    // Debug nesting info
    cout << endl;
    for (int i = kMaxDepth - depth ; i > 0 ; --i) {
      cout << " + ";
    }
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
