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

#include "board.h"

using std::array;
using std::bitset;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace chessy {

std::ostream& operator<<(std::ostream& os, const Square& square) {
  assert(square >= 0 && square < kSquares);
	string algebraic;
  algebraic += 'a' + File(square);
  algebraic += '1' + Rank(square);
  os << algebraic;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
  os << move.source << " -> " << move.dest;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os);
  return os;
}

bool CompareMoves(const Move& a, const Move& b) {
  if (a.score == b.score) {
    return std::rand() % 2 == 0;
  } else {
    return a.score > b.score;
  }
}

void AddMove(vector<Move>* moves, const Move& move) {
//  cout << "Adding move " << move << endl;
  moves->push_back(move);
  std::push_heap(moves->begin(), moves->end(), CompareMoves);
}

// TODO: Turn into transposition table
Move GetMove(vector<Move>* moves) {
  std::pop_heap(moves->begin(), moves->end(), CompareMoves);
  Move move = moves->back();
  moves->pop_back();
  return move;
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

} // chessy


int main(int argc, char** argv) {
  std::srand(static_cast<unsigned>(std::time(0)));
  chessy::Board board;
  for (;;) {
    cout << board << endl;
		const auto& move = chessy::BestMove(&board);
		cout << "Best Move Determined: " << move << endl;
    board.Update(move);
  }
  return 0;
}
