// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <ctime>
#include <signal.h>
#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "chessy.h"
#include "term.h"

static void OnQuit(int sig) {
  chessy::EndGame();
}

int main(int argc, char** argv) {
  google::SetUsageMessage("chessy [FLAGS]");
  google::SetVersionString("0.1");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  std::srand(static_cast<unsigned>(std::time(0)));
  signal(SIGINT, &OnQuit);
  signal(SIGABRT, &OnQuit);
  signal(SIGTERM, &OnQuit);
  std::cout << chessy::term::kClear;
  chessy::GameLoop();
  return 0;
}
