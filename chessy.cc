// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <cassert>
#include <cstdint>
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <string>
#include <vector>

using std::array;
using std::bitset;
using std::cout;
using std::endl;
using std::string;
using std::vector;

typedef int16_t Square;

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

struct Location {
  Location() : empty(true) {}
  Location(Color color, Piece piece, Square square)
      : empty(false), color(color), piece(piece), square(square) {}
  bool empty : 1;
  Color color : 1;
  Piece piece : 4;
  Square square : 16;
};

enum MoveType {
  kInvalid = 0,
  kRegular = 1,
  kAttack = 2,
  kCastle = 3,
  kCastleQueen = 4,
};

static const int kColors = 2;
static const int kPieces = 6;
static const Square kRow = 8;
static const Square kEast = 1;
static const Square kWest = -1;
static const Square kNorth = kRow;
static const Square kSouth = -kRow;
static const Square kSquares = kRow * kRow;

static const string kColorReset = "\x1b[0m";
static const string kColorGray = "\x1b[38;5;241m";
static const string kColorBlack = "\x1b[30m";
static const string kColorWhiteBg = "\x1b[47m";
static const string kColorGrayBg = "\x1b[48;5;246m";

typedef bitset<kSquares> Position;
typedef array<Position, kPieces * kColors> BitBoard;
typedef array<Location, kSquares> Locations;

static const array<string, kPieces * kColors> kPieceString = {{
  u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔",
  u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚",
}};

static const array<int, kPieces> kPieceValue = {{
  1,    // Pawn
  3,    // Knight
  3,    // Bishop
  5,    // Rook
  9,    // Queen
  666,  // King
}};

static const BitBoard kInitialBitBoard = {{
  Position("11111111") << kRow,        // White Pawn
  Position("01000010"),                // White Knight
  Position("00100100"),                // White Bishop
  Position("10000001"),                // White Rook
  Position("00001000"),                // White Queen
  Position("00010000"),                // White King
  Position("11111111") << (kRow * 6),  // Black Pawn
  Position("01000010") << (kRow * 7),  // Black Knight
  Position("00100100") << (kRow * 7),  // Black Bishop
  Position("10000001") << (kRow * 7),  // Black Rook
  Position("00001000") << (kRow * 7),  // Black Queen
  Position("00010000") << (kRow * 7),  // Black King
}};

static Color Toggle(Color color) {
  return (color == kWhite) ? kBlack : kWhite;
}

inline int Rank(Square square) {
  return square / kRow;
}

inline int File(Square square) {
  return square % kRow;
}

struct Move {
  Move() : type(kInvalid) {}
  explicit Move(MoveType type) : type(type) {}
  Move(MoveType type, Square source, Square dest)
      : type(type), source(source), dest(dest), captured(kPawn) {}
  Move(MoveType type, Square source, Square dest, Piece captured)
      : type(type), source(source), dest(dest), captured(captured) {}
  MoveType type : 4;
  Square source : 6;
  Square dest : 6;
  Piece captured : 3;
};

std::ostream& operator<<(std::ostream& os, const Move& move) {
  string from, to;
  from += 'a' + File(move.source);
  from += '1' + Rank(move.source);
  to += 'a' + File(move.dest);
  to += '1' + Rank(move.dest);
  os << from << " -> " << to;
  return os;
}

class Board {
 public:
  Board();
  Board(const Board&) = delete;
  void operator=(const Board&) = delete;
  void Update(const Move& move);
  void Undo(const Move& move);
  void DoSomethingLol();
  int Evaluate() const;
  void Print(std::ostream& os) const;

 private:
  vector<Move> PossibleMoves() const;
  Move TryMove(Square source, Square dest) const;
  void DirectionMoves(vector<Move>* res, Square source, Square dir) const;
  void PawnMoves(vector<Move>* res, Square square) const;
  void KnightMoves(vector<Move>* res, Square source) const;
  void BishopMoves(vector<Move>* res, Square source) const;
  void RookMoves(vector<Move>* res, Square source) const;
  void QueenMoves(vector<Move>* res, Square source) const;
  void KingMoves(vector<Move>* res, Square source) const;
  void ToggleColor();

  BitBoard board_;
  Locations locations_;
  Color color_;
};

Board::Board() : board_(kInitialBitBoard), color_(kWhite) {
  for (Square square = 0; square < kSquares; ++square) {
    for (int piece = 0; piece < kPieces; ++piece) {
      Piece sc_piece = static_cast<Piece>(piece);
      if (board_[piece][square]) {
        locations_[square] = Location(kWhite, sc_piece, square);
        break;
      }
      if (board_[piece + kPieces][square]) {
        locations_[square] = Location(kBlack, sc_piece, square);
        break;
      }
    }
  }
}

Move Board::TryMove(Square source, Square delta) const {
  Square dest = source + delta;
  // Horizontal wrapping check.
  int wrap = File(source) + File(delta + 4) - 4;
  if ((wrap < 0 || wrap >= kRow) ||
      (dest < 0 || dest >= kSquares)) {
    return Move(kInvalid);
  }
  const auto& loc = locations_[dest];
  if (loc.empty) {
    return Move(kRegular, source, dest);
  }
  if (loc.color == color_) {
    return Move(kInvalid);
  }
  return Move(kAttack, source, dest, loc.piece);
}

void Board::PawnMoves(vector<Move>* res, Square source) const {
  Square forward = (color_ == kWhite ? kNorth : kSouth);
  Move move;
  move = TryMove(source, forward + kEast);
  if (move.type == kAttack) {
    res->push_back(move);
  }
  move = TryMove(source, forward + kWest);
  if (move.type == kAttack) {
    res->push_back(move);
  }
  move = TryMove(source, forward);
  if (move.type == kRegular) {
    res->push_back(move);
    if ((color_ == kWhite && Rank(source) == 1) ||
        (color_ == kBlack && Rank(source) == 6)) {
      move = TryMove(source, forward * 2);
      if (move.type == kRegular) {
        res->push_back(move);
      }
    }
  }
  // TODO: Promotion
}

void Board::KnightMoves(vector<Move>* res, Square source) const {
  static const vector<Square> deltas = {
    kNorth + kNorth + kEast,
    kNorth + kNorth + kWest,
    kSouth + kSouth + kEast,
    kSouth + kSouth + kWest,
    kWest  + kWest  + kNorth,
    kWest  + kWest  + kSouth,
    kEast  + kEast  + kNorth,
    kEast  + kEast  + kSouth,
  };
  for (const auto delta : deltas) {
    Move move = TryMove(source, delta);
    if (move.type != kInvalid) {
      cout << move << endl;
      res->push_back(move);
    }
  }
}

void Board::DirectionMoves(vector<Move>* res, Square source, Square dir) const {
  Square delta = dir;
  Move move(kRegular);
  while (move.type == kRegular) {
    move = TryMove(source, delta);
    if (move.type != kInvalid) {
      res->push_back(move);
    }
    delta += dir;
  }
}

void Board::BishopMoves(vector<Move>* res, Square source) const {
  DirectionMoves(res, source, kNorth + kEast);
  DirectionMoves(res, source, kNorth + kWest);
  DirectionMoves(res, source, kSouth + kEast);
  DirectionMoves(res, source, kSouth + kWest);
}

void Board::RookMoves(vector<Move>* res, Square source) const {
  DirectionMoves(res, source, kNorth);
  DirectionMoves(res, source, kSouth);
  DirectionMoves(res, source, kEast);
  DirectionMoves(res, source, kWest);
}

void Board::QueenMoves(vector<Move>* res, Square source) const {
  BishopMoves(res, source);
  RookMoves(res, source);
}

void Board::KingMoves(vector<Move>* res, Square source) const {
  static const vector<Square> deltaz = {
    kNorth,
    kNorth + kEast,
    kNorth + kWest,
    kSouth,
    kSouth + kEast,
    kSouth + kWest,
    kEast,
    kWest,
  };
  for (const auto delta : deltaz) {
    Move move = TryMove(source, delta);
    if (move.type != kInvalid) {
      res->push_back(move);
    }
  }
}

vector<Move> Board::PossibleMoves() const {
  vector<Move> res;
  for (const auto& loc : locations_) {
    if (loc.empty || loc.color != color_) {
      continue;
    }
    switch (loc.piece) {
      case kPawn:
        PawnMoves(&res, loc.square);
        break;
      case kKnight:
        KnightMoves(&res, loc.square);
        break;
      case kBishop:
        BishopMoves(&res, loc.square);
        break;
      case kRook:
        RookMoves(&res, loc.square);
        break;
      case kQueen:
        QueenMoves(&res, loc.square);
        break;
      case kKing:
        KingMoves(&res, loc.square);
        break;
    }
  }
  return res;
}

void Board::Update(const Move& move) {
  cout << "moving: " << move << endl;
  auto& source = locations_[move.source];
  auto& dest = locations_[move.dest];
  assert(!source.empty);
  assert(dest.empty);
  source.empty = true;
  dest.empty = false;
  dest.piece = source.piece;
  dest.color = source.color;
  color_ = Toggle(color_);
  cout << dest.square << endl;
}

void Board::Undo(const Move& move) {
  color_ = Toggle(color_);
  auto& source = locations_[move.source];
  auto& dest = locations_[move.dest];
  source.empty = false;
  source.color = color_;
  source.piece = dest.piece;
  if (move.type == kAttack) {
    dest.empty = false;
    dest.color = Toggle(color_);
    dest.piece = move.captured;
  } else {
    dest.empty = true;
  }
}

int Board::Evaluate() const {
  int us = 0;
  int them = 0;
  for (const auto& loc : locations_) {
    if (loc.color == color_) {
      us += kPieceValue[loc.piece];
    } else {
      them += kPieceValue[loc.piece];
    }
  }
  return us - them;
}

// Print the chess board to an xterm-256color compatible terminal.
void Board::Print(std::ostream& os) const {
  array<string, kSquares> sboard;
  sboard.fill(" ");
  for (const auto& loc : locations_) {
    if (!loc.empty) {
      sboard[loc.square] = kPieceString[loc.piece + loc.color * kPieces];
    }
  }
  // for (Square square = 0; square < kSquares; ++square) {
  //   for (int piece = 0; piece < kPieces * 2; ++piece) {
  //     if (board_[piece][square]) {
  //       sboard[square] = kPieceString[piece];
  //       break;
  //     }
  //   }
  // }
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

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os);
  return os;
}

void Board::DoSomethingLol() {
  vector<Move> moves = PossibleMoves();
  cout << "possible moves: " << moves.size() << endl;
  assert(moves.size() == 20);
  Update(moves[0]);
}

int main(int argc, char** argv) {
  Board board;
  cout << board << endl;
  board.DoSomethingLol();
  cout << board << endl;
  return 0;
}
