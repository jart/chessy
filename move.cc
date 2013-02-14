// move.h - move representation
// 2013.02.08

#include "move.h"

#include <algorithm>

#include "constants.h"

using std::vector;

namespace chessy {

const array<string, kMoveTypes> kMoves = {{
  "Invalid",
  "Regular",
  "Capture",
  "Castle",
  "Queen-side Castle",
  "Check",
  "NullMove",
  "Tentative",
}};

Move::Move(MoveType type, Square source, Square dest, Piece captured)
    : source(source), dest(dest),
      type(type), captured(captured),
      priority(kPieceValues[captured]) {}

bool CompareMoves(const Move& a, const Move& b) {
  if (a.priority == b.priority) {
    return std::rand() % 2 == 0;
  } else {
    return a.priority < b.priority;
  }
}

void AddMove(Moves* moves, const Move& move) {
  //  cout << "Adding move " << move << endl;
  moves->push_back(move);
  std::push_heap(moves->begin(), moves->end(), CompareMoves);
}

// TODO: Turn into transposition table
Move GetMove(Moves* moves) {
  std::pop_heap(moves->begin(), moves->end(), CompareMoves);
  Move move = moves->back();
  moves->pop_back();
  return move;
}

string PrintMove(const Move& move) {
  string res = "";
  res += PrintSquare(move.source);
  res += "->";
  res += PrintSquare(move.dest);
  res += " [" + kMoves[move.type] + "]";
  return res;
}

std::ostream& operator<<(std::ostream& os, const MoveType& type) {
  os << kMoves[type];
  return os;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
  os << PrintMove(move);
  return os;
}

}  // namespace chessy
