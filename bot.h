// bot.h - bot processes and timing
// 2013.02.08

#ifndef CHESSY_BOT_H_
#define CHESSY_BOT_H_

#include <string>

namespace chessy {

  extern std::string g_last_message;

  // seconds
  const int kMaxThinkTime = 5;

  const int kMinScore = -99999;
  const int kMaxScore = 99999;
  const int kMaxDepth = 3;

  namespace bot {

    // TODO Chessy will continuously think in another thread.
    // "thinking" here is just improving the depth and accuracy of the game tree.
    void Think();

    void Sleep(int ms);

  }  // bot

}  // chessy

#endif  // CHESSY_BOT_H_
