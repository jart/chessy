// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <ctime>
#include <signal.h>
#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "bitmove.h"
#include "bitboard.h"
#include "board.h"
#include "chessy.h"
#include "square.h"
#include "term.h"

using std::cout;
using std::endl;
using std::string;

using namespace chessy;

static void OnQuit(int sig) {
  EndGame();
}

int main(int argc, char** argv) {
  google::SetUsageMessage("chessy [FLAGS]");
  google::SetVersionString("0.1");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  std::srand(static_cast<unsigned>(std::time(0)));
  signal(SIGINT, &OnQuit);
  InitBitmoves();

  // Board board;
  // board = Board(board, board.ComposeMove("e2", "e4"));
  // board = Board(board, board.ComposeMove("d7", "d5"));
  // board = Board(board, board.ComposeMove("f1", "b5"));
  // cout << board;
  // cout << board.IsChecking() << endl;

  cout << term::kClear;
  GameLoop();

  return 0;
}
