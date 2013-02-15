// board.h - board representation and interface
// 2013.02.08

#ifndef CHESSY_BOARD_H_
#define CHESSY_BOARD_H_

#include <array>
#include <bitset>
#include <forward_list>
#include <string>

#include "chessy.h"
#include "move.h"
#include "square.h"

namespace chessy {

enum BoardRepresentation {
  kBitBoards = 0,
  k0x88 = 1
};

// Consolidated details for a single square.
struct SquareState {
  SquareState() : empty(true) {}
  SquareState(Color color, Piece piece, int index)
      : empty(false), color(color), piece(piece), index(index) {}
  bool empty : 1;
  Color color : 1;
  Piece piece : 3;
  int index;  // Invariant: Should always match SquareTable index.
};

// During moves generation, we sometimes generate lists of squares without
// needing random lookup, but certainly requiring fast insertion and
// unidirectional traversal and space efficiency.
typedef std::forward_list<Square> Squares;

// 64-bit existential piece-type representation.
typedef std::bitset<64> BitBoard;
typedef std::array<BitBoard, kColors * kPieceTypes> BoardPosition;
typedef std::array<SquareState, 64> SquareTable;

typedef std::array<Square, kMaxPieces> PieceTable;

static const std::array<Piece, kMaxPieces> kPieceTableIndex = {{
  kPawn, kPawn, kPawn, kPawn, kPawn, kPawn, kPawn, kPawn,
  kKnight, kKnight, kBishop, kBishop, kRook, kRook,
  kQueen, kKing,
}};

// Units are in centi-pawns
const std::array<int, kPieceTypes> kPieceValues = {{
  100,  // Pawn
  300,  // Knight
  300,  // Bishop
  500,  // Rook
  900,  // Queen
  6666, // King
}};

const BitBoard kEmptyBoard = 0;

// TODO: Make the above not suck

// Board is mutable. There is typically only one "real" board object
// describing the current board state. However, there can be multiple
// think-space boards, each being branched upon.
class Board {
 public:
  Board();
  Board(const Board&) = delete;
  void operator=(const Board&) = delete;

  // Returns all possible moves for the current |color_| player.
  // The Moves contained in the vector are guaranteed to be valid, which means
  // each returned |Move| satisfies the following conditions:
  //
  // - Source square is occupied by one of the current player's pieces.
  // - Destination square is empty OR strictly occupied by an enemy piece
  // - The move does not put current player in check.
  //
  Moves PossibleMoves();

  // Update and Undo mutate the board. They must be symmetric.
  void Update(const Move& move);
  void Undo(const Move& move);
  void Reset();

  // Creates a move, qualified according to current board position.
  Move ComposeMove(Square source, Square dest);

  // Getters.
  Piece PieceAt(Square square) const;
  Color ColorAt(Square square) const;
  std::string StringAt(Square square) const;

  // Evaluated from the point of view of current player color.
  int Score() const;
  void Print(std::ostream& os, bool redraw) const;
  std::string color_str() const;

  inline Color color() { return color_; }
  inline void set_color(Color color) { color_ = color; }
  inline Move last_move() const { return last_move_; }

  BitBoard Friends();
  BitBoard Enemies();
  BitBoard GetBitBoard(Color color, Piece piece) const;

  bool SquareAttacked(Square square, Color color) const;
  // Square KingSquare(Color color) const;

  // Whether |square| was involved in the previous move.
  bool ActiveSquare(const Square square) const;
  // bool InCheck();

 private:
  // If we move a piece to |square|, what type of move will it be?  Does not
  // take into consideration en-passant nor castling nor queening.  Requires
  // that square is at least on the board.
  MoveType QualifyTarget(Square target);

  // Given a source square, return a set of Squares that some particular
  // piece-type could move to.
  Squares PawnTargets(Square square);
  Squares KnightTargets(Square square);
  Squares BishopTargets(Square square);
  Squares RookTargets(Square square);
  Squares QueenTargets(Square square);
  Squares KingTargets(Square square);

  void SlidingTargets(Squares* res, Square source, Square delta);
  void SetAttacked(Square square);
  BitBoard PositionMask(Color color) const;

  BoardPosition board_;
  // Existence maps of white and black.
  BitBoard friends_ = kEmptyBoard;  // Memoized masks
  BitBoard enemies_ = kEmptyBoard;

  // BitBoards for existential/boolean checks.
  BitBoard white_pieces = kEmptyBoard;
  BitBoard white_attack = kEmptyBoard;
  BitBoard black_pieces = kEmptyBoard;
  BitBoard black_attack = kEmptyBoard;

  // Squares currently under attack by current player. Automatically filled
  // during calls to PossibleMoves(), to speed up certain checks.
  BitBoard attacked_ = kEmptyBoard;

  PieceTable piece_table[2];

  SquareTable square_table_;  // Indexed by Square
  Color color_;

  // Square king_square_[kColors];

  Move last_move_ = kBadMove;
};

bool ValidMove(Board* board, const Move move);
bool InCheck(Board* board, Color color);
bool Checkmate(Board* board);
bool Stalemate(Board* board);

std::ostream& operator<<(std::ostream& os, const Board& board);

}  // namespace chessy

#endif  // CHESSY_BOARD_H_
