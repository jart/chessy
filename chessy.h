// chessy - a chess engine by justine and serene
// 2013.02.03

#ifndef CHESSY_H_
#define CHESSY_H_

#include <cassert>
#include <cstdint>
#include <string>

namespace chessy {

enum GameMode {
  kMirror = 0,  // Bot plays its self
  kHuman = 1,  // Bot plays YOU!
};

enum GameState {
  kNone = 0,
  kPlaying = 1,
  kPaused = 2,
};

extern GameMode g_mode;
extern GameState g_state;

extern int g_dbg;  // Debug level from 0-2

// search counters
extern int g_nodes_searched;
extern int g_nodes_pruned;

void GameLoop();

}  // chessy

#endif  // CHESSY_BOT_H_
