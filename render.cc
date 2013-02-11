// 2013.02.09

#include "chessy.h"
#include "board.h"
#include "move.h"
#include "bot.h"
#include "render.h"

using std::cout;
using std::endl;

// using namespace chessy::terminal;

namespace chessy {
 
  void Board::Print(std::ostream& os) const {
    array<string, square::kTotal> sboard;
    sboard.fill(" ");
    for (const auto& state : square_table_) {
      if (!state.empty) {
        sboard[state.index] = kPieceString[PieceIndex(state.color, state.piece)];
      }
    }
    // Print in reverse - rank 8 is top, 1 is bottom
    for (int y = kRow - 1; y >= 0; --y) {
      os << terminal::kGray << y + 1 << " ";
      os << terminal::kBlack;
      for (int x = 0; x < kRow; ++x) {
        // Checkerboard color
        os << ((x + y) % 2 ? (terminal::kWhiteBg) : (terminal::kGrayBg));
        os << sboard[y * kRow + x] << " ";
      }
      os << terminal::kReset << endl;
    }
    os << terminal::kGray << "  ";
    for (int x = 0; x < kRow; ++x) {
      os << static_cast<char>('A' + x) << " ";
    }
    os << terminal::kReset;
  }


  // cout << board
  std::ostream& operator<<(std::ostream& os, const Board& board) {
    board.Print(os);
    return os;
  }

  // cout << piece
  std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    os << names::kPieces[piece];
    return os;
  }

  // cout << square
  std::ostream& operator<<(std::ostream& os, const Square& square) {
    if (!square::Valid(square)) {
      os << "NA(" << std::hex << static_cast<int>(square) << ")";
      return os;
    }
    string algebraic;
    algebraic += 'a' + (int)square::File(square);
    algebraic += '1' + (int)square::Rank(square);
    os << algebraic;
    return os;
  }

  // cout << movetype
  std::ostream& operator<<(std::ostream& os, const MoveType& type) {
    os << names::kMoveTypes[type];
    return os;
  }

  // cout << move
  std::ostream& operator<<(std::ostream& os, const Move& move) {
    if (g_dbg > 0) {
      os << "[" << move.type << "";
      if (kAttack == move.type) {
        os << " " << names::kPieces[move.captured];
      }
      os << "] ";
    }
    // os << "[" << move.type << move.captured << "] "
    os << move.source << "->" << move.dest;
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
