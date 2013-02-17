// chessy - a chess engine by justine and serene
// 2013.02.03

#ifndef CHESSY_H_
#define CHESSY_H_

#include <string>

namespace chessy {

enum GameMode {
  kMirror = 0,  // Bot plays itself.
  kHuman = 1,  // Bot plays YOU!
};

enum GameState {
  kNone = 0,
  kPlaying = 1,
  kPaused = 2,
};

// Play the game.
void GameLoop();

// Stop the game loop (usually from a signal handler).
void EndGame();

}  // namespace chessy

#endif  // CHESSY_H_
