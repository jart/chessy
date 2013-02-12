// bot.h - primary chessy 'AI' logics
// 2013.02.08

#ifndef CHESSY_BOT_H_
#define CHESSY_BOT_H_

#include "board.h"

namespace chessy {
namespace bot {

  const int kMaxDepth = 3;
  const int kMinScore = -99999;
  const int kMaxScore = 99999;
  const int kMaxThinkTime = 5;        // seconds

  // The "main" root-level entry point to *some* algorithm inside operandi::
  int Seed(Board* board);
  void Chillax(int ms);

  // TODO: Chessy will continuously think in multiple threads
  // "thinking" here is just improving the depth and accuracy of the game tree.
  void Think();

// Collection of minimax-style gametree optmization algorithms.
namespace operandi {
 
  int NegaMax(Board* board, int depth, int alpha, int beta);

  // TODO: Once there are multiple algorithms, run tests to determine the 
  // efficacy of each, and possibly interchange different algorithms for 
  // different situations.
  
  // These algorithms could be fun:
  // - NegaScout
  // - NegaC*
  // - MTD(f)
  // - SSS*
  // - Dual*

}  // operandi
}  // bot

// Bot-logic debug helpers
namespace dbg {

  // search metrics
  extern int g_branches_searched;
  extern int g_branches_pruned;

  // render:: based
  void ChessyBeginsThinking(Board* board, int branches);
  void NewBest(int score, const Move& move);
  void ChessyProgress();
  void ChessyFinishesThinking(int score);

  void BranchWindow(Board* board, int depth, int branches, int alpha, int beta);
  void Heuristic(Board* board, int score);

  void NewBranches(int increment);
  void Negamaxed(Board* board, int depth, int alpha);
  void BetaPrune(Board* board, int depth, int beta, int pruned);

  // [2+] detailed gametree step-through
  void DepthIndent(int depth);  // Deliminate current depth of gametree

  void Step(int min_level);  // Block and wait for human input to step through

}  // dbg
}  // chessy


#endif  // CHESSY_BOT_H_
