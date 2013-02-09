// bot.h - bot processes and timing
// 2013.02.08

#ifndef CHESSY_BOT_H_
#define CHESSY_BOT_H_

namespace chessy {

// seconds
const kMaxThinkTime = 5;

// Chessy will continuously think in another thread
void Think();

#endif  // CHESY_BOT_H_
