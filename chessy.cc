// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <cstdint>
#include <array>
#include <iostream>
#include <string>

using std::array;
using std::cout;
using std::endl;
using std::string;

static const int kColors = 2;
static const int kPieces = 6;
static const int kWidth = 8;
static const int kSquares = kWidth * kWidth;
static const string kColorReset = "\x1b[0m";
static const string kColorGray = "\x1b[38;5;241m";
static const string kColorBlack = "\x1b[30m";
static const string kColorWhiteBg = "\x1b[47m";
static const string kColorGrayBg = "\x1b[48;5;246m";

typedef uint64_t Position;

enum Color {
  kWhite = 0,
  kBlack = 1,
};

enum Piece {
  kPawn = 0,
  kKnight = 1,
  kBishop = 2,
  kRook = 3,
  kQueen = 4,
  kKing = 5,
};

// struct Move {
//   Color color : 1;
//   Piece piece : 3;
//   unsigned int current : 6;
//   unsigned int dest : 6;
// };

static const array<string, kPieces * kColors> kPieceString = {{
  u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔",
  u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚",
}};

static const array<Position, kPieces> kInitial = {{
  0xff,  // Pawn
  0x42,  // Knight
  0x24,  // Bishop
  0x81,  // Rook
  0x08,  // Queen
  0x10,  // King
}};

class Board {
 public:
  Board();
  void Reset();
  void Print(std::ostream& os) const;

 private:
  array<Position, kPieces * kColors> board_;
};

Board::Board() {
  Reset();
}

// Clear the board and set all pieces to their starting positions.
void Board::Reset() {
  board_.fill(0x0);
  board_[kPawn] |= kInitial[kPawn] << kWidth;
  board_[kPawn + kPieces] |= kInitial[kPawn] << (kSquares - kWidth * 2);
  for (int piece = kKnight; piece < kPieces; ++piece) {
    board_[piece] |= kInitial[piece];
    board_[piece + kPieces] |= kInitial[piece] << (kSquares - kWidth);
  }
}

// Print the chess board to an xterm-256color compatible terminal.
void Board::Print(std::ostream& os) const {
  array<string, kSquares> sboard;
  sboard.fill(" ");
  for (int bit = 0; bit < kSquares; ++bit) {
    for (int piece = kPawn; piece < kPieces * 2; ++piece) {
      if ((board_[piece] >> bit) & 1) {
        sboard[bit] = kPieceString[piece];
        break;
      }
    }
  }
  for (int y = kWidth - 1; y >= 0; --y) {
    os << kColorGray << y + 1 << " ";
    os << kColorBlack;
    for (int x = 0; x < kWidth; ++x) {
      os << ((x + y) % 2 ? kColorWhiteBg : kColorGrayBg);
      os << sboard[y * kWidth + x] << " ";
    }
    os << kColorReset << endl;
  }
  os << kColorGray << "  ";
  for (int x = 0; x < kWidth; ++x) {
    os << static_cast<char>('A' + x) << " ";
  }
  os << kColorReset;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os);
  return os;
}

int main(int argc, char** argv) {
  Board board;
  cout << board << endl;
  return 0;
}
