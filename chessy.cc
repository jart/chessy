// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include "chessy.h"

#include <cassert>
#include <iostream>
#include <string>

#include "board.h"
#include "bot.h"
#include "move.h"
#include "render.h"
#include "square.h"
#include "term.h"

using std::cout;
using std::endl;
using std::string;

// TODO: Use gflags

namespace chessy {

GameMode g_mode = kMirror;
GameState g_state = kNone;

Move ChessyMove(Board* board) {
  int score = bot::kMinScore;   // what a pessimist
  Move best = kBadMove;

  Moves moves = board->PossibleMoves();
  dbg::ChessyBeginsThinking(board, moves.size());

  for (const auto& move : moves) {

    // Root-level move selection is AGNOSTIC to the internal algorithm.
    // Here we directly track the "best move", whereas the recursive internal
    // algorithm focuses on improving "scores" by neurotically branching and
    // verifying a-b windows as much as possible.

    board->Update(move);
    int val = bot::Seed(board);   // TODO: parallelize the search
    board->Undo(move);

    if (val > score) {
      score = val;
      best = move;
      dbg::NewBest(score, move);
    }

    dbg::ChessyProgress();    // (maybe) track root progress
    if (kPlaying != g_state)  // Breaking out early
      return best;
  }

  dbg::ChessyFinishesThinking(score);
  return best;
}

// Human Move Obtainium.

Move ValidateNotation(const string& input) {
  if (input.size() != 4)
    return kBadMove;

  Square source = Getx88(input[0], input[1]);
  Square dest = Getx88(input[2], input[3]);

  if (!(Valid(source) && Valid(dest)))
    return kBadMove;

  return Move(kTentative, source, dest);
}

bool operator==(const Move &a, const Move &b) {
  return ((a.type == b.type) &&
          (a.captured == b.captured) &&
          (a.source == b.source) &&
          (a.dest == b.dest));
}

Move HumanMove(Board* board) {
  string input;
  const auto& possible = board->PossibleMoves();
  bool valid_move = false;
  while (!valid_move && kPlaying == g_state) {
    input = render::HumanMovePrompt();
    if ("quit" == input) {
      g_state = kNone;
      return kBadMove;
    }
    // Rando
    if (tolower(input[0]) == 'r') {
      render::Status("You RANDOMLY moved:  ");
      return possible[std::rand() % possible.size()];
    }

    const auto& temp = ValidateNotation(input);
    if (kInvalidMove == temp.type) {
      render::Status("Smith notation please. (Example: a1b2)");
      continue;
    }

    const auto& move = board->ComposeMove(temp.source, temp.dest);
    for (const auto& pmove : possible) {
      if (move == pmove) {
        // Check that this move is *actually* valid.
        valid_move = true;
        render::Status("You moved:  ");
        return move;
      }
    }
    render::Status(PrintMove(move) + " is invalid for the " +
                   "current board position.");
  }
  return kBadMove;
}

void EndGame() {
  // Either stops a match in progress
  if (kPlaying == g_state) {
    //std::cin.putback('\n');
    //fflush(stdin);
    render::Status("Game interrupted!");
    g_state = kNone;
    return;
  }

  // Or the entire program
  cout << term::kClear;
  render::ChessyNewMsg("Goodbye. <3\n");
  cout << term::kShowCursor << term::kReset;
  exit(0);
}


void DetermineGameType() {
  static int games_started = 0;
  string prompt = "How about a nice game of chess?";
  // or global thermonuclear war
  if (games_started > 0)
    prompt = "Shall we rematch?";

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
       << endl << endl;

  Board board;
  render::Everything(&board);
  string chessy_greeting = "Greetings, Professor. ";

  while(true) {
    render::ChessyNewMsg(chessy_greeting);
    DetermineGameType();

    board.Reset();
    render::Everything(&board);
    if (kHuman == g_mode) {
      render::ChessyNewMsg("Your move, dear ;)");
    } else {
      render::Status("Chessy will play Chessy! :D");
      bot::Chillax(1500);
    }

    while(kPlaying == g_state) {
      Move move = kBadMove;
      if (kHuman == g_mode && kWhite == board.color()) {
        move = HumanMove(&board);
        if (kPlaying != g_state) {
          render::Status("You forfeited.");
          break;
        }

        assert(kInvalidMove != move.type);  // no cheating, dear

      } else {
        move = ChessyMove(&board);
        render::ChessyMsg("\n\n\t Result:  ");
      }

      // Resultant move will print at the current cursor position, which is
      // set from above as either for the Human or for Chessy.
      string move_str = board.StringAt(move.source) + " " + PrintMove(move);
      cout << move_str;

      board.Update(move);  // ph0 realz
      render::UpdateBoard(&board, move);

      // TODO: Make this not suck.
      if (InCheck(&board, board.color())) {
        cout << endl << "CHECK!" << endl;
      }

      // end-game situations
      // TODO: make this not blow
      if (Checkmate(&board)) {
        render::Status("CHECKMATE xD" +
                       ColorString(Toggle(board.color())) + " wins.");
        g_state = kNone;

      } else if (Stalemate(&board)) {
        render::Status("STALEMATE");
        g_state = kNone;
      }

      if (g_mode == kHuman)   // slow-down for the silly humans
        bot::Chillax(500);
    }
    chessy_greeting = "That was fun! ";
  }
}

int PieceIndex(Color color, Piece piece) {
  return piece + (color == kWhite? 0 : kPieceTypes);
}

}  // namespace chessy
