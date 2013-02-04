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

const int kColors = 2;
const int kPieces = 6;
const int kWidth = 8;
const int kSquares = kWidth * kWidth;
const string kColorReset = "\x1b[0m";
const string kColorGray = "\x1b[38;5;241m";
const string kColorBlack = "\x1b[30m";
const string kColorWhiteBg = "\x1b[47m";
const string kColorGrayBg = "\x1b[48;5;246m";

typedef uint64_t Position;
typedef array<Position, kPieces * kColors> Board;

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

const array<string, kPieces * kColors> kPieceString = {
  "♙", "♘", "♗", "♖", "♕", "♔",
  "♟", "♞", "♝", "♜", "♛", "♚",
};

const array<Position, kPieces> kInitial = {
  0xff,  // Pawn
  0x42,  // Knight
  0x24,  // Bishop
  0x81,  // Rook
  0x08,  // Queen
  0x10,  // King
};

Board g_board;

void Reset() {
  g_board.fill(0x0);
  g_board[kPawn] |= kInitial[kPawn] << kWidth;
  g_board[kPawn + kPieces] |= kInitial[kPawn] << (kSquares - kWidth * 2);
  for (int piece = kKnight; piece < kPieces; ++piece) {
    g_board[piece] |= kInitial[piece];
    g_board[piece + kPieces] |= kInitial[piece] << (kSquares - kWidth);
  }
}

void Print() {
  array<string, kSquares> sboard;
  sboard.fill(" ");
  for (int bit = 0; bit < kSquares; ++bit) {
    for (int piece = kPawn; piece < kPieces * 2; ++piece) {
      if ((g_board[piece] >> bit) & 1) {
        sboard[bit] = kPieceString[piece];
        break;
      }
    }
  }
  for (int y = kWidth - 1; y >= 0; --y) {
    cout << kColorGray << y + 1 << " ";
    cout << kColorBlack;
    for (int x = 0; x < kWidth; ++x) {
      cout << ((x + y) % 2 ? kColorWhiteBg : kColorGrayBg);
      cout << sboard[y * kWidth + x] << " ";
    }
    cout << kColorReset << endl;
  }
  cout << kColorGray << "  ";
  for (int x = 0; x < kWidth; ++x) {
    cout << static_cast<char>('A' + x) << " ";
  }
  cout << kColorReset << endl;
}

int main(int argc, char** argv) {
  Reset();
  Print();
  return 0;
}
