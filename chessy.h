// chessy - a chess engine by justine and serene
// 2013.02.03

#ifndef CHESSY_H_
#define CHESSY_H_

#include <cassert>
#include <cstdint>
#include <string>

using std::string;

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

void GameLoop();

}

#endif  // CHESY_BOT_H_
