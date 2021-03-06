// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include "chessy.h"

#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>

#include <glog/logging.h>

#include "board.h"
#include "bot.h"
#include "render.h"
#include "square.h"
#include "term.h"

using std::cout;
using std::endl;
using std::string;

namespace chessy {

const string kChessyHappy = term::kGreen + "Q(^_^ Q)";
const string kChessySad = term::kRed + u8"\u00F7(-_- \u00F7)";

static GameMode g_mode = kMirror;
static GameState g_state = kNone;

static void Chillax(int ms) {
  const struct timespec req = {
    .tv_sec = ms / 1000,
    .tv_nsec = (ms % 1000) * 1000000L,
  };
  fflush(stdout);  // Ensure renders up-to-date.
  nanosleep(&req, NULL);
}

static void ChessyBeginsThinking(const Board& board, int branches) {
  // BLACK chessy msgs on top, WHITE chessy msgs on bottom.
  if (kBlack == board.color()) {
    render::ChessyNewMsg("BLACK THOUGHTS~");
  } else {
    render::ChessyNewMsgMirror("WHITE THOUGHTS~");
  }
  render::ChessyMsg(  // Basic metrics.
      "  |d|="    + term::i2s(kMaxDepth) +
      "  |M|="    + term::i2s(branches) +
      "  |eval|=" + term::i2s(board.score()) +
      "\n\t" + term::kPink + u8"\u00A7");  // Begin progress bar.
}

static void NewBest(int score, const Bitmove& move) {
  string hval = (score >= 0 ?  // GREEN+ RED-
                 term::kGreen + "+" :
                 term::kRed) + term::i2s(score);
  render::ChessyMsg("{" + hval + term::kPink + "}");
  // Further |move| detail
  render::ChessyMsg(
      "Best move thus far: " + move.ToString() +
      " (s-val=" + term::i2s(score) + ")\n\t");
}

static void ChessyProgress() {
  if (kHuman == g_mode)  // Slow down for stupid humans.
    Chillax(1);
  render::ChessyMsg(u8"\u00BB");  // Indicator
  fflush(stdout);
}

static void ChessyFinishesThinking(int score) {
  // Chessy has happy metrics!
  float savings = static_cast<float>(g_branches_pruned) /
                  static_cast<float>(g_branches_searched) * 100;
  const string& face = (score >= 0) ? kChessyHappy : kChessySad;
  render::ChessyMsg(
      u8"\u00A7 " + face + term::kPink +
      "\n\t\t total branches: " + term::i2s(g_branches_searched) +
      "\n\t\t total pruned:   " + term::i2s(g_branches_pruned) +
      "\n\t\t search savings: " + term::i2s(savings) + "%");
}

static Bitmove ChessyMove(const Board& board, const Bitmoves& moves) {
  int score = kMinScore;  // What a pessimist!
  Bitmove best = Bitmove::kInvalid;
  ChessyBeginsThinking(board, moves.size());
  for (const auto& move : moves) {
    // Root-level move selection is AGNOSTIC to the internal algorithm.
    // Here we directly track the "best move", whereas the recursive internal
    // algorithm focuses on improving "scores" by neurotically branching and
    // verifying a-b windows as much as possible.
    int val = Think(Board(board, move));
    if (val > score) {
      score = val;
      best = move;
      NewBest(score, move);
      if (val == kMaxScore) {  // Checkmate! <('.'<)
        break;
      }
    }
    ChessyProgress();  // (maybe) track root progress
    if (kPlaying != g_state)  // Breaking out early
      return best;
  }
  ChessyFinishesThinking(score);
  return best;
}

static bool ValidateNotation(const string& input,
                             Square* source, Square* dest) {
  if (input.size() != 4)
    return false;
  *source = Square(input.substr(0, 2));
  *dest = Square(input.substr(2, 2));
  return (source->IsValid() && dest->IsValid());
}

static const Bitmove& HumanMove(const Board& board, const Bitmoves& moves) {
  string input;
  while (kPlaying == g_state) {
    input = render::HumanMovePrompt();
    if ("quit" == input) {
      g_state = kNone;
      return Bitmove::kInvalid;
    }
    if (tolower(input[0]) == 'r') {
      render::Status("You RANDOMLY moved:  ");
      return moves[std::rand() % moves.size()];
    }
    Square source, dest;
    if (!ValidateNotation(input, &source, &dest)) {
      render::Status("Smith notation please. (Example: a1b2)");
      continue;
    }
    const Bitmove& move = board.ComposeMove(source, dest);
    if (move.IsValid()) {
      render::Status("You moved:  ");
      return move;
    }
    render::Status(move.ToString() + " is invalid for the " +
                   "current board position.");
  }
  return Bitmove::kInvalid;
}

void EndGame() {
  // Either stops a match in progress.
  if (kPlaying == g_state) {
    //std::cin.putback('\n');
    //fflush(stdin);
    render::Status("Game interrupted!");
    g_state = kNone;
    return;
  }
  // Or the entire program.
  cout << term::kClear;
  render::ChessyNewMsg("Goodbye. <3\n");
  cout << term::kShowCursor << term::kReset;
  exit(0);
}

static void DetermineGameType() {
  static int games_started = 0;
  string prompt = "How about a nice game of chess?";
  // or global thermonuclear war...
  if (games_started > 0) {
    prompt = "Shall we rematch?";
  }
  if (render::YesNoPrompt(prompt)) {
    g_mode = kHuman;
    render::Status("You are WHITE and Chessy is BLACK.");
  } else {
    render::ChessyNewMsg("Alrighty. How about a lovely ");
    if (render::YesNoPrompt("2-chessy 1-board demonstration?")) {
      g_mode = kMirror;
    } else {
      EndGame();
    }
  }
  games_started++;
  g_state = kPlaying;  // Let the games begin!
}

void GameLoop() {
  cout << "Chessy v0.1" << endl
       << "A chess bot by Justine and Serene ^_^"
       << endl
       << endl;

  string chessy_greeting = "Greetings, Professor. ";
  while (true) {
    Board board;
    render::Everything(board);
    render::ChessyNewMsg(chessy_greeting);
    DetermineGameType();

    render::Everything(board);
    if (kHuman == g_mode) {
      render::ChessyNewMsg("Your move, dear ;)");
    } else {
      render::Status("Chessy will play Chessy! :D");
      Chillax(1500);
    }

    while (kPlaying == g_state) {
      Bitmoves moves = board.PossibleMoves();
      if (moves.size() == 0) {
        render::Status("Checkmate <3");
        break;
      }
      Bitmove move;
      if (kHuman == g_mode && kWhite == board.color()) {
        move = HumanMove(board, moves);
        if (kPlaying != g_state) {
          render::Status("You forfeited.");
          break;
        }
        CHECK(move.IsValid());  // No cheating, dear.
      } else {
        move = ChessyMove(board, moves);
        render::ChessyMsg("\n\n\t Result:  ");
      }

      // Resultant move will print at the current cursor position, which is
      // set from above as either for the Human or for Chessy.
      string move_str = (board.GetPiece(move.source).Describe() + " " +
                         move.ToString());
      cout << move_str;

      board = Board(board, move);  // ph0 realz
      render::UpdateBoard(board, move);

      // TODO: Make this not suck.
      if (board.IsChecking()) {
        cout << endl << "CHECK!" << endl;
      }

      // // end-game situations
      // // TODO: make this not blow
      // if (Checkmate(&board)) {
      //   render::Status("CHECKMATE xD" +
      //                  GetColorName(Toggle(board.color())) + " wins.");
      //   g_state = kNone;

      // } else if (Stalemate(&board)) {
      //   render::Status("STALEMATE");
      //   g_state = kNone;
      // }

      if (g_mode == kHuman)   // slow-down for the silly humans
        Chillax(500);
    }
    chessy_greeting = "That was fun! ";
  }
}

}  // namespace chessy
