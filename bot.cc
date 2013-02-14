// bot.cc - primary chessy 'AI' logic implementation
// 2013.02.09

#include <iostream>

#include <glog/logging.h>

#include "board.h"
#include "bot.h"
#include "move.h"
#include "render.h"
#include "term.h"

using std::string;

namespace chessy {

int g_branches_searched = 0;
int g_branches_pruned = 0;

#define TLOG \
  VLOG(2) << string((kMaxDepth - depth) * 2, ' ')

int Think(Board* board) {
  return -NegaMax(board, kMaxDepth, kMinScore, kMaxScore);
}

// Maximizes the negation of the enemy player's positions.
int NegaMax(Board* board, int depth, int alpha, int beta) {
  Moves moves = board->PossibleMoves();
  TLOG << moves.size() << "-< ("
       << ColorString(Toggle(board->color())) << " "
       << board->last_move()
       << ") a[" << alpha
       << "] b[" << beta << "] >- ";
  if (depth == 0 || moves.size() == 0) {
    int score = board->Score();
    TLOG << "h-val(" << board->color_str() << ")=" << score;
    // TODO: Quiescent search if last_move_ is "exciting".
    return score;
  }
  g_branches_searched += moves.size();
  for (const auto& move : moves) {
    board->Update(move);
    int val = -NegaMax(board, depth - 1, -beta, -alpha);
    board->Undo(move);
    // Beta pruning skips remaining branches, because the current sub-tree is
    // now guaranteed to be futile (at least within the current depth).
    if (val >= beta) {
      g_branches_pruned += moves.size();
      TLOG << "<-- b-pruned(" << board->color_str() << ")=" << beta;
      return val;
    }
    // Alpha just maximizes the negation of the next moves.
    if (val >= alpha) {
      alpha = val;
    }
  }
  TLOG << "<--- a-negamaxed(" << board->color_str() << ")=" << alpha;
  return alpha;
}

}  // namespace chessy
