// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <signal.h>

#include "chessy.h"
#include "bot.h"
#include "board.h"
#include "move.h"
#include "render.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace chessy {

  GameMode g_mode = kMirror;
  GameState g_state = kNone;

  // Set g_dbg >= 2 for search tree step-through verbosity
  int g_dbg = 1;
  int g_nodes_searched = 0;
  int g_nodes_pruned = 0;

  int NegaMax(Board* board, int depth, int alpha, int beta) {
    Moves moves = board->PossibleMoves();

    // Debug with branch factor and window
    if (g_dbg > 1) {
      DepthCout(depth);
      const auto& last_move = board->last_move();
      cout << "" << moves.size() << "-< ("
           << ColorString(Toggle(board->color())) << " " << last_move 
           << ") a[" << alpha
           << "] b[" << beta << "] >- ";
    }

    if (depth == 0 || moves.size() == 0) {
      int score = board->Score(); // * ((board->color() != kWhite) ? 1 : -1);
      if (g_dbg > 1) {
        cout << "h-val(" << board->color_str() << ")=" << score;
      }
      // cout << "SCORE=" << score << endl;
      return score;
    }

    g_nodes_searched += moves.size();

    for (const auto& move : moves) {
      board->Update(move);
      int val = -NegaMax(board, depth - 1, -beta, -alpha);
      board->Undo(move);

      // Beta pruning skips remaining branches, because the current sub-true is 
      // now guaranteed to offer no improvement
      if (val >= beta) {
        if (g_dbg > 1) {
          DepthCout(depth);
          cout << "<-- b-pruned(" << board->color_str() << ")=" << val;
        }
        g_nodes_pruned += moves.size();;
        return val;
      }

      // Alpha just maximizes the negation of the next moves
      if (val >= alpha) {
        alpha = val;
      }
    }
    if (g_dbg > 1) {
      DepthCout(depth);
      cout << "<--- a-negamaxed(" << board->color_str() << ")=" << alpha;
    }
    return alpha;
  }

  Move BestMove(Board* board) {
    Move best;
    int score = kMinScore;
    g_nodes_searched = 0;
    g_nodes_pruned = 0;

    // Chessy's log position depends on color
    if (kBlack == board->color()) {
      render::ChessyNewMsg("BLACK THOUGHTS~");
    } else {
      render::ChessyNewMsgMirror("WHITE THOUGHTS~");
    }

    Moves moves = board->PossibleMoves();

    if (g_dbg > 0) {
      render::ChessyMsg("  |d|=" + i2s(kMaxDepth) +
                        "  |M|=" + i2s(moves.size()));
      render::ChessyMsg(
          "  |eval|=" + i2s(board->Score()) + "\n\t" +
          term::kPink + "§");
    }

    for (const auto& move : moves) {

      // Root node move-selection loop is agnostic to the internal algorithm,
      // and associates scores with moves rather than pruning windows.
      board->Update(move);
      int next = -NegaMax(board, kMaxDepth, kMinScore, kMaxScore);
      board->Undo(move);

      if (next > score) {
        if (g_dbg > 2) {
          render::ChessyMsg(
              "Best move thus far: " + PrintMove(move) +
              " (s-val=" + i2s(next) + ")\n\t");
        }
        score = next;
        best = move;
        if (g_dbg > 0) {
          string hval = (next >= 0 ? term::kGreen : term::kRed) + i2s(next);
          render::ChessyMsg("{" + hval + term::kPink + "}");
        }
      }

      // If debugging the search tree, pause for keypresses between branches
      if (g_dbg > 2) {
        string lol;
        std::getline(std::cin, lol);
      }

      if (kHuman == g_mode)   // Temporary slowing of pace so we can see things!
        bot::Sleep(1);
      if (g_dbg > 0) {
        render::ChessyMsg("»");
        fflush(stdout);
      }
    }

    // happy metrics!
    if (g_dbg > 0) {
      float savings = 100 * static_cast<float>(g_nodes_pruned) /
                      static_cast<float>(g_nodes_searched);
      string happy = score >= 0 ? 
          term::kGreen + "Q(^_^ Q)":
          term::kRed   + "÷(-_- ÷)";

      render::ChessyMsg("§ " + happy + term::kPink + "\n");
      render::ChessyMsg("\t\t total branches: " + i2s(g_nodes_searched) + "\n");
      render::ChessyMsg("\t\t total pruned:   " + i2s(g_nodes_pruned) + "\n");
      render::ChessyMsg("\t\t search savings:   " + i2s(savings) + "%");
    }
    return best;
  }

  inline Color Toggle(Color color) {
    return (color == kWhite) ? kBlack : kWhite;
  }

  int PieceIndex(Color color, Piece piece) {
    return piece + (color == kWhite? 0 : kPieceTypes);
  }

  Move ValidateNotation(const string& input) {
    if (input.size() != 4)
      return kInvalidMove;

    Square source = square::Getx88(input[0], input[1]);
    Square dest = square::Getx88(input[2], input[3]);

    if (!(square::Valid(source) && square::Valid(dest)))
      return kInvalidMove;

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
      input = render::HumanPrompt();
      if ("quit" == input) {
        g_state = kNone;
        return kInvalidMove;
      }
      if (tolower(input[0]) == 'r') {
        render::Status("You are doing a RANDOM MOVE! ;)");
        return possible[std::rand() % possible.size()];
      }
         
      const auto& temp = ValidateNotation(input);
      if (kInvalid == temp.type) {
        render::Status("Smith notation please. (Example: a1b2)");
        continue;
      }

      const auto& move = board->ComposeMove(temp.source, temp.dest);
      for (const auto& pmove : possible) {
        if (move == pmove) {
          // Check that this move is *actually* valid.
          valid_move = true;
          return move;
        }
      }
      render::Status(PrintMove(move) + " is invalid for the " +
          "current board position.");
    }
    return kInvalidMove;
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
  void GameLoop() {
    cout << "Chessy v0.1" << endl
         << "A chess bot by Justine and Serene ^_^"
         << endl << endl;


    Board board;
    render::Everything(&board);

    while(true) {
      render::ChessyNewMsg("Hello Professor. ");
      if (render::YesNoPrompt("How about a nice game of chess?")) {
        g_mode = kHuman;
        render::ChessyNewMsg("Your move, dear ;)");
        render::Status("You are WHITE and Chessy is BLACK.");
      } else {
        g_mode = kMirror;
        render::Status("Chessy is playing Chessy! :)");
      }

      g_state = kPlaying;
      while(kPlaying == g_state) {

        Move move = kInvalidMove;
        if (g_mode == kHuman && kWhite == board.color()) {
          // HUMAN MOVE
          move = HumanMove(&board);
          if (kPlaying != g_state) {
            render::Status("You forfeited against Chessy.");
            break;
          }
          assert(kInvalid != move.type);
          render::Status("You moved:  ");

        } else {
          // CHESSY MOVE
          move = BestMove(&board);
          render::ChessyMsg("\n\n\t Result: ");
        }
        string move_str = board.StringAt(move.source) + " " + PrintMove(move);
        cout << move_str;

        board.Update(move);
        render::UpdateBoard(&board, move);

        if (InCheck(&board, board.color())) {
          cout << endl << "CHECK!" << endl;
        }

        // --- End game conditions ---
        if (Checkmate(&board)) {
          cout << endl << "CHECKMATE xD"
               << ColorString(Toggle(board.color())) << " Wins." << endl;
          g_state = kNone;

        } else if (Stalemate(&board)) {
          render::Status("STALEMATE");
          g_state = kNone;
        }

        if (g_mode == kHuman)
          bot::Sleep(500);
      }

      render::ChessyNewMsg("That was fun! ");
      if (!render::YesNoPrompt("Another round?")) {
        EndGame();
        break;
      }

      board.Reset();
      render::Everything(&board);
    }

  }

} // chessy

void sighandler(int sig) {
  chessy::EndGame();
}

int main(int argc, char** argv) {
  std::srand(static_cast<unsigned>(std::time(0)));
  signal(SIGINT, &sighandler);
  signal(SIGABRT, &sighandler);
  signal(SIGTERM, &sighandler);
  cout << chessy::term::kClear;
  chessy::GameLoop();
  return 0;
}
