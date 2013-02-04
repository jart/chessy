// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <cstdint>
#include <array>
#include <iostream>
#include <string>
#include <vector>

using std::array;
using std::cout;
using std::endl;
using std::string;
using std::vector;

typedef uint64_t Position;
typedef array<Position, 12> Board;

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

const array<string, 12> kPieceString = {
  "♙", "♘", "♗", "♖", "♕", "♔",
  "♟", "♞", "♝", "♜", "♛", "♚",
};

const array<Position, 6> kInitialRows = {
  0xff,  // Pawn
  0x42,  // Knight
  0x24,  // Bishop
  0x81,  // Rook
  0x08,  // Queen
  0x10,  // King
};

Board g_board;

void Reset() {
  g_board.fill(0);
  for (int n = 0; n < 6; ++n) {
    if (n == kPawn) {
      g_board[n] |= kInitialRows[n] << 8;
      g_board[n + 6] |= kInitialRows[n] << (64 - 16);
    } else {
      g_board[n] |= kInitialRows[n];
      g_board[n + 6] |= kInitialRows[n] << (64 - 8);
    }
  }
}

void Checker(bool is_white) {
  if (is_white) {
    cout << "\x1b[47;30m";
  } else {
    cout << "\x1b[48;5;246;30m";
  }
}

void EndLine() {
  cout << "\x1b[0m" << endl;
}

void Print() {
  array<string, 64> sboard;
  sboard.fill(" ");
  for (int bit = 0; bit < 64; ++bit) {
    for (int piece = 0; piece < 12; ++piece) {
      if ((g_board[piece] >> bit) & 1) {
        sboard[bit] = kPieceString[piece];
        break;
      }
    }
  }
  for (int y = 7; y >= 0; --y) {
    cout << "\x1b[38;5;241m" << y + 1 << " ";
    for (int x = 0; x < 8; ++x) {
      Checker((x + (y % 2)) % 2);
      cout << sboard[y * 8 + x] << " ";
    }
    EndLine();
  }

  // Last row letters.
  cout << "\x1b[38;5;241m  ";
  for (int x = 0; x < 8; ++x) {
    cout << static_cast<char>('A' + x) << " ";
  }
  EndLine();
}

int main(int argc, char** argv) {
  Reset();
  Print();
  return 0;
}
