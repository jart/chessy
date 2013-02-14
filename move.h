// move.h - move representation
// 2013.02.08

#ifndef CHESSY_MOVE_H_
#define CHESSY_MOVE_H_

#include <array>
#include <queue>
#include <vector>

#include "chessy.h"
#include "square.h"

namespace chessy {

enum MoveType {
  kInvalidMove = 0,
  kRegular = 1,
  kAttack = 2,
  kCastle = 3,
  kCastleQueen = 4,
  kCheck = 5,
  kNullMove = 6,  // For null-move pruning and check conditions.
  kTentative = 7,
};

const int kMoveTypes = 8;

// Moves depend on the Board to be applied upon. Not all moves are valid for
// any particular board position.
struct Move {
  Move() : type(kInvalidMove) {}
  explicit Move(MoveType type) : type(type) {}
  Move(MoveType type, Square source, Square dest) :
      source(source), dest(dest),
      type(type), captured(kNoPiece),
      priority(0) {}
  Move(MoveType type, Square source, Square dest, Piece captured);

  Square source;
  Square dest;
  MoveType type : 3;
  Piece captured : 3;
  int priority;  // Move ordering.
};

const Move kBadMove = Move();

// Use a priority queue for move ordering.
// Comparison function required for *Move Ordering*.
typedef std::priority_queue<Move> MoveHeap;
typedef std::vector<Move> Moves;
bool operator==(const Move &a, const Move &b);

void AddMove(Moves* moves, const Move& move);
Move GetMove(Moves* moves);

std::string PrintMove(const Move& move);

std::ostream& operator<<(std::ostream& os, const MoveType& type);
std::ostream& operator<<(std::ostream& os, const Move& move);

}  // namespace chessy

#endif  // CHESSY_MOVE_H_
