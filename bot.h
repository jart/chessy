// bot.h - bot processes and timing
// 2013.02.08

#ifndef CHESSY_BOT_H_
#define CHESSY_BOT_H_

namespace chessy {

// seconds
const int kMaxThinkTime = 5;

const int kMinScore = -99999;
const int kMaxScore = 99999;
const int kMaxDepth = 2;

// Chessy will continuously think in another thread
void Think();

}  // chessy

#endif  // CHESSY_BOT_H_
