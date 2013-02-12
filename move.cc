// move.h - move representation
// 2013.02.08

#include <vector>
#include <algorithm>  // push_heap

#include "move.h"
#include "constants.h"

using std::vector;

namespace chessy {

  Move::Move(MoveType type, Square source, Square dest, Piece captured) :
      source(source), dest(dest),
      type(type), captured(captured),
      priority(kPieceValues[captured]) {}

  namespace square {

    bool Valid(Square square) {
      return !(square & kInvalidMask);
    }

    Square Getx88(Offset rank, Offset file) {
      return kRank * rank + file;  
    }
    Square Getx88(char f, char r) {
      Offset file = tolower(f) - 'a';
      if (file < 0 || file >= kRow)
        return kInvalid;

      Offset rank = r - '1';
      if (rank < 0 || rank >= kRow)
        return kInvalid;
      return Getx88(rank, file);
    }
    Square Getx88(int index) {
      if (index < 0 || index >= kTotal)
        return kInvalid;
      return kRank * (index >> 3) + index % kRow; 
    }
    int Index(Square square) {
      assert(Valid(square));
      return square % kRow + Rank(square) * kRow; 
    }
    Offset Rank(Square square) {
      return square >> kRankShift;  // 4th through 7th bit
    }

    Offset File(Square square) {
      return square & kFileMask;    // First 3 bits
    }
  }

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

}  // chessy
