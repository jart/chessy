// bot.cc - primary chessy 'AI' logic implementation
// 2013.02.09

#include <iostream>

#include <glog/logging.h>

#include "bitboard.h"
#include "bitmove.h"
#include "board.h"
#include "bot.h"
#include "render.h"
#include "term.h"

using std::string;

namespace chessy {

int g_branches_searched = 0;
int g_branches_pruned = 0;

#define TLOG \
  VLOG(2) << string((kMaxDepth - depth) * 2, ' ')

int Think(const Board& board) {
  return -NegaMax(board, kMaxDepth, kMinScore, kMaxScore);
}

// Maximizes the negation of the enemy player's positions.
int NegaMax(const Board& board, int depth, int alpha, int beta) {
  Bitmoves moves = board.PossibleMoves();
  TLOG << moves.size()
       << "-< (" << Toggle(board.color())
       // << " "    << board.last_move()
       << ") a[" << alpha
       << "] b[" << beta
       << "] >- ";
  if (moves.size() == 0) {
    TLOG << "h-val(" << board.color() << ")=" << kMaxScore;
    return kMaxScore;
  }
  if (depth == 0) {
    int score = board.score();
    TLOG << "h-val(" << board.color() << ")=" << score;
    // TODO: Quiescent search if last_move_ is "exciting".
    return score;
  }
  g_branches_searched += moves.size();
  for (const Bitmove& move : moves) {
    int val = -NegaMax(Board(board, move), depth - 1, -beta, -alpha);
    // Beta pruning skips remaining branches, because the current sub-tree is
    // now guaranteed to be futile (at least within the current depth).
    if (val >= beta) {
      g_branches_pruned += moves.size();
      TLOG << "<-- b-pruned(" << board.color() << ")=" << beta;
      return val;
    }
    // Alpha just maximizes the negation of the next moves.
    if (val >= alpha) {
      alpha = val;
    }
  }
  TLOG << "<--- a-negamaxed(" << board.color() << ")=" << alpha;
  return alpha;
}

}  // namespace chessy
