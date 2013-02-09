// 2013.02.09

#include "render.h"
#include "chessy.h"
#include "board.h"
#include "bot.h"


using std::cout;
using std::endl;

namespace chessy {

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os);
  return os;
}

std::ostream& operator<<(std::ostream& os, const Square& square) {
  if (square < 0 || square >= kSquares) {
    os << "NA(" << static_cast<int>(square) << ")";
    return os;
  }
  string algebraic;
  algebraic += 'a' + File(square);
  algebraic += '1' + Rank(square);
  os << algebraic;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
  if (g_dbg > 1) {
    os << "[" << kMoveTypeNames[move.type] << "";
    if (kAttack == move.type) {
      os << " " << kPieceNames[move.captured];
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
  string name = kPieceNames[piece];
  return ColorString(ColorAt(square)) + " " + name;
}

string Board::color_str() const {
  return ColorString(color_);
}

void Board::Print(std::ostream& os) const {
  array<string, kSquares> sboard;
  sboard.fill(" ");
  for (const auto& state : square_table_) {
    if (!state.empty) {
      sboard[state.square] = kPieceString[PieceIndex(state.color, state.piece)];
    }
  }
  for (int y = kRow - 1; y >= 0; --y) {
    os << kColorGray << y + 1 << " ";
    os << kColorBlack;
    for (int x = 0; x < kRow; ++x) {
      os << ((x + y) % 2 ? kColorWhiteBg : kColorGrayBg);
      os << sboard[y * kRow + x] << " ";
    }
    os << kColorReset << endl;
  }
  os << kColorGray << "  ";
  for (int x = 0; x < kRow; ++x) {
    os << static_cast<char>('A' + x) << " ";
  }
  os << kColorReset;
}

}  // chessy
