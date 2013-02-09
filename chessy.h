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

GameMode g_mode = kMirror;
GameState g_state = kNone;

int g_dbg = 1;  // Debug level from 0-2

// search counters
int g_nodes_searched = 0;
int g_nodes_pruned = 0;

void GameLoop();

}  // chessy

#endif  // CHESSY_BOT_H_
