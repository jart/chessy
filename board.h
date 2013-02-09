// board.h - board representation constants
// 2013.02.08

#ifndef CHESSY_BOARD_H_
#define CHESSY_BOARD_H_

#include <array>
#include <bitset>
#include <string>
#include <vector>

using std::array;
using std::bitset;
using std::string;
using std::vector;

namespace chessy {

// Select different board representation to optimize for different 
// situations.
enum BoardRepresentation {
  kBitBoards = 0,
  k0x88 = 1
};

// Color and Piece types
enum Color {
  kWhite = 0,
  kBlack = 1,
};
const int kColors = 2;

Color Toggle(Color color);

enum Piece {
  kPawn = 0,
  kKnight = 1,
  kBishop = 2,
  kRook = 3,
  kQueen = 4,
  kKing = 5,
  kNoPiece = 6,
};
const int kPieceTypes = 6;
const int kMaxPieces = 16;

// Some terminals are happy, and some are SAD!
#define UNICODE
#ifdef UNICODE
const array<string, kPieceTypes * kColors> kPieceString = {{
  u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔",
  u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚",
}};
#else
const array<string, kPieceTypes * kColors> kPieceString = {{
   "P", "N", "B", "R", "Q", "K",
   "P", "N", "B", "R", "Q", "K",
}};
#endif

const array<string, kPieceTypes> kPieceNames = {{
  "Pawn",
  "Knight",
  "Bishop",
  "Rook",
  "Queen",
  "Knight",
}};

typedef int8_t Offset;

int PieceIndex(Color color, Piece piece);

// Squares index 0-63
typedef int8_t Square;
const Square kRow = 8;
const Square kSquares = kRow * kRow;
const Square kInvalidSquare = -1;
typedef vector<Square> SquareSet;

int Rank(Square square);
int File(Square square);

struct SquareState {
  SquareState() : empty(true), square(kInvalidSquare) {}
  SquareState(Square square) : empty(true), square(square) {}
  SquareState(Color color, Piece piece, Square square)
      : empty(false), color(color), piece(piece), square(square) {}
  bool empty : 1;
  Color color : 1;
  Piece piece : 3;
  Square square : 7;  // Invariant: Should always match SquareTable index.
};

// Color-agnostic
typedef array<Square, kMaxPieces> PieceTable;
static const array<Piece, kMaxPieces> kPieceTableIndex = {{
  kPawn, kPawn, kPawn, kPawn, kPawn, kPawn, kPawn, kPawn,
  kKnight, kKnight, kBishop, kBishop, kRook, kRook,
  kQueen, kKing,
}};

const string kColorReset = "\x1b[0m";
const string kColorGray = "\x1b[38;5;241m";
const string kColorBlack = "\x1b[30m";
const string kColorWhiteBg = "\x1b[47m";
const string kColorGrayBg = "\x1b[48;5;246m";

// 64-bit existential piece-type representation
typedef bitset<kSquares> BitBoard;
typedef array<BitBoard, 2 * kPieceTypes> BoardPosition;
typedef array<SquareState, kSquares> SquareTable;
const BitBoard kEmpty = 0;

// <Rank, File> vector representation
struct Delta {
  Delta(Offset rank, Offset file) : rank(rank), file(file) {}

  Delta operator+(const Delta& other) const {
    return Delta(rank + other.rank, file + other.file);
  }
  void operator+=(const Delta& other) {
    rank += other.rank;
    file += other.file;
  }
  Delta operator*(int factor) const {
    return Delta(rank * factor, file * factor);
  }

  Offset rank;
  Offset file;
};

// Movement constants
const Delta kU ( 1,  0);
const Delta kD (-1,  0);
const Delta kR ( 0,  1);
const Delta kL ( 0, -1);
const Delta kUR( 1,  1);
const Delta kUL( 1, -1);
const Delta kDR(-1,  1);
const Delta kDL(-1, -1);
const array<Delta, 8> kKnightDeltas = {{
  kU + kUR, kU + kUL,
  kD + kDR, kD + kDL,
  kR + kUR, kR + kDR,
  kL + kUL, kL + kDL,
}};
const array<Delta, 4> kOrthogonals = {{
  kU, kD, kR, kL,
}};
const array<Delta, 4> kDiagonals = {{
  kUR, kUL, kDR, kDL,
}};
const array<Delta, 8> kOmnigonals = {{
  kU, kD, kR, kL, kUR, kUL, kDR, kDL,
}};

// Value-resolution in centi-pawns.
const array<int, kPieceTypes> kPieceValue = {{
  100,    // Pawn
  300,    // Knight
  300,    // Bishop
  500,    // Rook
  900,    // Queen
  666,  // King
}};

const BoardPosition kInitialBoardPosition = {{
  BitBoard("11111111") << kRow,        // White Pawn
  BitBoard("01000010"),                // White Knight
  BitBoard("00100100"),                // White Bishop
  BitBoard("10000001"),                // White Rook
  BitBoard("00001000"),                // White Queen
  BitBoard("00010000"),                // White King
  BitBoard("11111111") << (kRow * 6),  // Black Pawn
  BitBoard("01000010") << (kRow * 7),  // Black Knight
  BitBoard("00100100") << (kRow * 7),  // Black Bishop
  BitBoard("10000001") << (kRow * 7),  // Black Rook
  BitBoard("00001000") << (kRow * 7),  // Black Queen
  BitBoard("00010000") << (kRow * 7),  // Black King
}};

enum MoveType {
  kInvalid = 0,
  kRegular = 1,
  kAttack = 2,
  kCastle = 3,
  kCastleQueen = 4,
  kTentative = 5,
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
  int score;
};
bool operator==(const Move &a, const Move &b);
const Move kInvalidMove = Move();

void AddMove(vector<Move>* moves, const Move& move);
Move GetMove(vector<Move>* moves);


// Board is mutable
class Board {
 public:
  Board();
  Board(const Board&) = delete;
  void operator=(const Board&) = delete;

  vector<Move> PossibleMoves() const;

  // Update and Undo must be symmetric.
  void Update(const Move& move);
  void Undo(const Move& move);

  Move ComposeMove(Square source, Square dest) const;
  MoveType QualifySquare(Square square);

  Piece PieceAt(Square square) const;
  Color ColorAt(Square square) const;
  string StringAt(Square square) const;

  // Evaluated from the point of view of current player color.
  int Score() const;
  void Print(std::ostream& os) const;

  inline Color color() { return color_; }
  inline void set_color(Color color) { color_ = color; }

  BitBoard Friends();
  BitBoard Enemies();
  BitBoard GetBitBoard(Color color, Piece piece) const;

 private:
  Move TryMove(Square source, Delta delta) const;

  void SlidingMoves(vector<Move>* res, Square source, Delta delta) const;
  void PawnMoves(vector<Move>* res, Square square) const;
  void KnightMoves(vector<Move>* res, Square source) const;
  void BishopMoves(vector<Move>* res, Square source) const;
  void RookMoves(vector<Move>* res, Square source) const;
  void QueenMoves(vector<Move>* res, Square source) const;
  void KingMoves(vector<Move>* res, Square source) const;

  BitBoard PositionMask(Color color) const;

  BoardPosition board_;
  BitBoard friends_ = kEmpty;  // Memoized masks
  BitBoard enemies_ = kEmpty;

  PieceTable piece_table;
  SquareTable square_table_;  // Indexed by Square
  Color color_;
};

}  // chessy

#endif  // CHESSY_BOARD_H_
