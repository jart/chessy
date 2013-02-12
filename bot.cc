// 2013.02.09

#include <string>
#include <ctime>

#include "bot.h"

namespace chessy {

  std::string g_last_message = "Hi! <3";

  namespace bot {

    void Think() {
      // meow
    }

    void Sleep(int ms) {
      struct timespec req = {0};
      req.tv_sec = ms / 1000;
      req.tv_nsec = (ms % 1000) * 1000000L;
      fflush(stdout);   // We care about up-to-date renders
      nanosleep(&req, NULL);
    }
  }  // bot
}  // chessy
