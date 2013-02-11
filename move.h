// move.h - move representation
// 2013.02.08

#ifndef CHESSY_MOVE_H_
#define CHESSY_MOVE_H_

#include <forward_list>
#include <queue>
#include <vector>

#include "chessy.h"

using std::vector;
using std::forward_list;
using std::priority_queue;

namespace chessy {

  // --- Bits! ---
  const Bitx88 kInvalidMask = 0x88;
  const Offset kRow = 8;

  namespace square {

    const int kTotal        = 0x40;  // 64
    const Offset kRank      = 0x10;  // 16
    const Offset kFile      = 0x01;
    const Offset kRankShift = 4;
    const Bitx88 kFileMask  = 0x07;
    const Square kInvalid   = 0x80;  // Default "invalid" square

    // Conversions between Rank, File numbers, Indices, and 0x88
    Offset Rank(Square square);
    Offset File(Square square);
    Square Getx88(Offset rank, Offset file);
    Square Getx88(char f, char r);
    Square Getx88(int index);
    int Index(Square square);

    bool Valid(Square square);
  }

  // During moves generation, we sometimes generate lists of squares without 
  // needing random lookup, but certainly requiring fast insertion and 
  // unidirectional traversal and space efficiency
  typedef forward_list<Square> Squares;

  namespace deltas {
    // --- Offsets between 0x88 squares (+) ---
    
    const Offset kU = square::kRank;
    const Offset kD = -kU;
    const Offset kR = square::kFile;
    const Offset kL = -kR;
    const Offset kUR = kU + kR;
    const Offset kUL = kU + kL;
    const Offset kDR = kD + kR;
    const Offset kDL = kD + kL;

    const array<Offset, 4> kOrthogonal = {{
      kU, kD, kR, kL,
    }};
    const array<Offset, 4> kDiagonal = {{
      kUR, kUL, kDR, kDL,
    }};
    const array<Offset, 8> kOmnigonal = {{
      kU, kD, kR, kL, kUR, kUL, kDR, kDL,
    }};

    // The 8 'L' shapes
    const array<Offset, 8> kKnight = {{
      kU + kUR, kU + kUL,
      kD + kDR, kD + kDL,
      kR + kUR, kR + kDR,
      kL + kUL, kL + kDL,
    }};

  }


  // --- Move representation ---
  enum MoveType {
    kInvalid = 0,
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

    Move() : type(kInvalid) {}
    explicit Move(MoveType type) : type(type) {}
    Move(MoveType type, Square source, Square dest) :
        source(source), dest(dest),
        type(type), captured(kNoPiece),
        priority(0) {}
    Move(MoveType type, Square source, Square dest, Piece captured);  // move.cc

    Square source;
    Square dest;
    MoveType type : 3;
    Piece captured : 3;
    int priority;  // Move ordering

  };
  const Move kInvalidMove = Move();

  // Use a priority queue for move ordering
  // Comparison function required for *Move Ordering*
  typedef priority_queue<Move> MoveHeap;
  typedef vector<Move> Moves;
  bool operator==(const Move &a, const Move &b);

  void AddMove(Moves* moves, const Move& move);
  Move GetMove(Moves* moves);


} // chessy

#endif  // CHESSY_MOVE_H_
