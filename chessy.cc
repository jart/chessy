// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <cassert>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <vector>

using std::array;
using std::bitset;
using std::cout;
using std::endl;
using std::string;
using std::vector;

typedef int8_t Value;
typedef int8_t Square;
typedef std::pair<Square, Square> Delta;

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
  Piece piece : 3;
  Square square : 7;
};

static const int kColors = 2;
static const int kPieces = 6;
static const Square kRow = 8;
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

static const array<Value, kPieces> kPieceValue = {{
  1,   // Pawn
  3,   // Knight
  3,   // Bishop
  5,   // Rook
  9,   // Queen
  50,  // King
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

Color Toggle(Color color) {
  return (color == kWhite) ? kBlack : kWhite;
}

inline int Rank(Square square) {
  return square / kRow;
}

inline int File(Square square) {
  return square % kRow;
}

enum MoveType {
  kInvalid = 0,
  kRegular = 1,
  kAttack = 2,
  kCastle = 3,
  kCastleQueen = 4,
};

struct Move {
  Move() : type(kInvalid) {}
  explicit Move(MoveType type) : type(type) {}
  Move(MoveType type, Square source, Square dest)
      : type(type), source(source), dest(dest), captured(kPawn) {}
  Move(MoveType type, Square source, Square dest, Piece captured)
      : type(type), source(source), dest(dest), captured(captured),
        score(kPieceValue[captured]) {}
  MoveType type : 3;
  Square source : 7;
  Square dest : 7;
  Piece captured : 3;
  Value score;
};

bool CompareMoves(const Move& a, const Move& b) {
  if (a.score == b.score) {
    return std::rand() % 2 == 0;
  } else {
    return a.score > b.score;
  }
}

void AddMove(vector<Move>* moves, const Move& move) {
  moves->push_back(move);
  std::push_heap(moves->begin(), moves->end(), CompareMoves);
}

Move GetMove(vector<Move>* moves) {
  std::pop_heap(moves->begin(), moves->end(), CompareMoves);
  Move move = moves->back();
  moves->pop_back();
  return move;
}

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
  int Score() const;
  vector<Move> PossibleMoves() const;
  void Print(std::ostream& os) const;

  inline Color color() { return color_; }
  inline void set_color(Color color) { color_ = color; }

 private:
  Move TryMove(Square source, Square dx, Square dy) const;
  void DirectionMoves(vector<Move>* res, Square source,
                      Square dx, Square dy) const;
  void PawnMoves(vector<Move>* res, Square square) const;
  void KnightMoves(vector<Move>* res, Square source) const;
  void BishopMoves(vector<Move>* res, Square source) const;
  void RookMoves(vector<Move>* res, Square source) const;
  void QueenMoves(vector<Move>* res, Square source) const;
  void KingMoves(vector<Move>* res, Square source) const;

  BitBoard board_;
  Locations locations_;
  Color color_;
};

Board::Board() : board_(kInitialBitBoard), color_(kWhite) {
  for (int square = 0; square < kSquares; ++square) {
    locations_[square].square = square;
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

Move Board::TryMove(Square source, Square dx, Square dy) const {
  if ((Rank(source) + dy < 0 || Rank(source) + dy >= kRow) ||
      (File(source) + dx < 0 || File(source) + dx >= kRow)) {
    return Move(kInvalid);
  }
  Square dest = source + dx + dy * kRow;
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
  Square forward = (color_ == kWhite ? 1 : -1);
  Move move;
  move = TryMove(source, 1, forward);
  if (move.type == kAttack) {
    AddMove(res, move);
  }
  move = TryMove(source, -1, forward);
  if (move.type == kAttack) {
    AddMove(res, move);
  }
  move = TryMove(source, 0, forward);
  if (move.type == kRegular) {
    AddMove(res, move);
    if ((color_ == kWhite && Rank(source) == 1) ||
        (color_ == kBlack && Rank(source) == 6)) {
      move = TryMove(source, 0, forward * 2);
      if (move.type == kRegular) {
        AddMove(res, move);
      }
    }
  }
  // TODO: Promotion
}

void Board::KnightMoves(vector<Move>* res, Square source) const {
  static const vector<Delta> deltas = {
    {  2,  1 },
    {  2, -1 },
    { -2,  1 },
    { -2, -1 },
    {  1, -2 },
    { -1, -2 },
    {  1,  2 },
    { -1,  2 },
  };
  for (const auto updown : deltas) {
    Move move = TryMove(source, updown.first, updown.second);
    if (move.type != kInvalid) {
      AddMove(res, move);
    }
  }
}

void Board::DirectionMoves(vector<Move>* res, Square source,
                           Square dx, Square dy) const {
  Square dxm = dx;
  Square dym = dy;
  Move move(kRegular);
  while (move.type == kRegular) {
    move = TryMove(source, dxm, dym);
    if (move.type != kInvalid) {
      AddMove(res, move);
    }
    dxm += dx;
    dym += dy;
  }
}

void Board::BishopMoves(vector<Move>* res, Square source) const {
  DirectionMoves(res, source,  1,  1);
  DirectionMoves(res, source, -1,  1);
  DirectionMoves(res, source,  1, -1);
  DirectionMoves(res, source, -1, -1);
}

void Board::RookMoves(vector<Move>* res, Square source) const {
  DirectionMoves(res, source,  0,  1);
  DirectionMoves(res, source,  0, -1);
  DirectionMoves(res, source,  1,  0);
  DirectionMoves(res, source, -1,  0);
}

void Board::QueenMoves(vector<Move>* res, Square source) const {
  BishopMoves(res, source);
  RookMoves(res, source);
}

void Board::KingMoves(vector<Move>* res, Square source) const {
  static const vector<Delta> deltaz = {
    {  1,  1 },
    {  1,  0 },
    {  1, -1 },
    {  0,  1 },
    {  0,  0 },
    {  0, -1 },
    { -1,  1 },
    { -1,  0 },
    { -1, -1 },
  };
  for (const auto updown : deltaz) {
    Move move = TryMove(source, updown.first, updown.second);
    if (move.type != kInvalid) {
      AddMove(res, move);
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
  assert(move.type != kInvalid);
  // if (move.captured == kKing) {
  //   switch (color_) {
  //     case kWhite:
  //       cout << "White wins!" << endl;
  //       exit(0);
  //     case kBlack:
  //       cout << "Black wins!" << endl;
  //       exit(0);
  //   }
  // }
  auto& source = locations_[move.source];
  auto& dest = locations_[move.dest];
  assert(!source.empty);
  source.empty = true;
  dest.empty = false;
  dest.piece = source.piece;
  dest.color = source.color;
  color_ = Toggle(color_);
}

void Board::Undo(const Move& move) {
  assert(move.type != kInvalid);
  auto& source = locations_[move.source];
  auto& dest = locations_[move.dest];
  color_ = Toggle(color_);
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

int Board::Score() const {
  int us = 0;
  int them = 0;
  for (const auto& loc : locations_) {
    if (loc.empty) {
      continue;
    }
    if (loc.color == color_) {
      us += kPieceValue[loc.piece];
    } else {
      them += kPieceValue[loc.piece];
    }
  }
  return us - them;
}

void Board::Print(std::ostream& os) const {
  array<string, kSquares> sboard;
  sboard.fill(" ");
  for (const auto& loc : locations_) {
    if (!loc.empty) {
      sboard[loc.square] = kPieceString[loc.piece + loc.color * kPieces];
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

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os);
  return os;
}

int NegaMax(Board* board, int depth, int alpha, int beta, Move* o_best) {
  vector<Move> moves = board->PossibleMoves();
  if (depth == 0 || moves.size() == 0) {
    int score = board->Score() * ((board->color() != kWhite) ? 1 : -1);
    // cout << "SCORE=" << score << endl;
    return score;
  }
  while (moves.size() > 0) {
    const Move move = GetMove(&moves);
    board->Update(move);
    // cout << *board
    //      << endl
    //      << "alpha=" << alpha
    //      << " beta=" << beta
    //      << " depth=" << depth
    //      << " moves=" << moves.size()
    //      << endl;
    // string lol;
    // std::getline(std::cin, lol);
    int val = -NegaMax(board, depth - 1, -beta, -alpha, NULL);
    board->Undo(move);
    if (val >= beta) {
      return val;
    }
    if (val >= alpha) {
      alpha = val;
      if (o_best) {
        *o_best = move;
      }
    }
  }
  return alpha;
}

Move BestMove(Board* board) {
  Move best;
  NegaMax(board, 5, -9999, +9999, &best);
  return best;
}

int main(int argc, char** argv) {
  std::srand(static_cast<unsigned>(std::time(0)));
  Board board;
  for (;;) {
    cout << board << endl;
    board.Update(BestMove(&board));
  }
  return 0;
}
