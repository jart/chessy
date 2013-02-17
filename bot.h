// bot.h - primary chessy 'AI' logics
// 2013.02.08

#ifndef CHESSY_BOT_H_
#define CHESSY_BOT_H_

namespace chessy {

class Board;

const int kMaxDepth = 2;
const int kMinScore = -99999;
const int kMaxScore = 99999;
const int kMaxThinkTime = 5;  // seconds

extern int g_branches_searched;
extern int g_branches_pruned;

int Think(const Board& board);
int NegaMax(const Board& board, int depth, int alpha, int beta);

// TODO: Once there are multiple algorithms, run tests to determine the
// efficacy of each, and possibly interchange different algorithms for
// different situations.

// These algorithms could be fun:
// - NegaScout
// - NegaC*
// - MTD(f)
// - SSS*
// - Dual*

}  // namespace chessy

#endif  // CHESSY_BOT_H_
