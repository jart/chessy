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

    cout << "Chessy is thinking..." << endl << endl;

    Moves moves = board->PossibleMoves();
    for (const auto& move : moves) {

      // Root node move-selection loop is agnostic to the internal algorithm,
      // and associates scores with moves rather than pruning windows.
      board->Update(move);
      int next = -NegaMax(board, kMaxDepth, kMinScore, kMaxScore);
      board->Undo(move);

      if (next > score) {
        if (g_dbg > 0) {
          cout << "Chessy: best move thus far: " << move
               << " (s-val=" << next << ")" << endl;
        }
        score = next;
        best = move;
      }
      string lol;

      // If debugging the search tree, pause for keypresses between branches
      if (g_dbg > 1)
        std::getline(std::cin, lol);
    }

    if (g_dbg > 0) {
      float savings = 100 * static_cast<float>(g_nodes_pruned) /
                      static_cast<float>(g_nodes_searched);
      cout << "Chessy finished thinking!" << endl
           << "   total branches: " << g_nodes_searched << endl
           << "   total pruned:   " << g_nodes_pruned << endl
           << "   search savings: " << savings << "%" << endl;
    }
    return best;
  }

  inline Color Toggle(Color color) {
    return (color == kWhite) ? kBlack : kWhite;
  }

  int PieceIndex(Color color, Piece piece) {
    return piece + (color == kWhite? 0 : kPieceTypes);
  }

  Move ValidateAlgebraicMove(const string& input) {
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
    bool valid_move = false;
    string input;

    while (!valid_move) {
      cout << "Please input a move in algebraic notation: ";
      std::cin >> input;
      const auto& temp = ValidateAlgebraicMove(input);
      if (kInvalid == temp.type) {
        cout << "\"" << input << "\" is invalid algebraic notation."
             << endl
             << "(Example: a1b2)"
             << endl;
        continue;
      }

      const auto& move = board->ComposeMove(temp.source, temp.dest);
      for (const auto& pmove : board->PossibleMoves()) {
        if (move == pmove) {
          // Check that this move is *actually* valid.
          valid_move = true;
          return move;
        }
      }
      cout << move << " is invalid for the "
           << "current board position." << endl;
    }
    return kInvalidMove;
  }

  bool PromptUser(string msg) {
    cout << msg << " [y/n]: ";
    string option;
    std::getline(std::cin, option);
    return std::toupper(option[0]) == 'Y';
  }

  void GameLoop() {
    cout << "Chessy v0.1" << endl
         << "A chess bot by Justine and Serene ^_^"
         << endl << endl;

    Board board;

    while(true) {
      if (PromptUser("Chessy: Hello Professor. How about a nice game of chess?")) {
        g_mode = kHuman;
        cout << "You are WHITE and Chessy is BLACK." << endl;
      } else {
        cout << "Chessy is playing Chessy!" << endl;
      }

      g_state = kPlaying;
      while(kPlaying == g_state) {
        cout << board << endl;
        Move move = kInvalidMove;
        if (g_mode == kHuman && kWhite == board.color()) {
          move = HumanMove(&board);
          assert(kInvalid != move.type);
          cout << "You moved: ";
        } else {
          move = BestMove(&board);
          cout << endl << "Chessy: ";
        }
        cout << "[" << board.StringAt(move.source) << "] " << move << endl;
        board.Update(move);

        if (InCheck(&board, board.color())) {
          cout << endl << "CHECK!" << endl;
        }
        // End game conditions
        if (Checkmate(&board)) {
          cout << endl << "CHECKMATE!"
               << ColorString(Toggle(board.color())) << " Wins." << endl;
          g_state = kNone;

        } else if (Stalemate(&board)) {
          cout << endl << "STALEMATE." << endl;;
          g_state = kNone;
        }
      }
      if (!PromptUser("Chessy: Reset?")) {
        cout << "Bye bye!" << endl;
        break;
      }
    }

  }

} // chessy


int main(int argc, char** argv) {
  std::srand(static_cast<unsigned>(std::time(0)));
  chessy::GameLoop();
  return 0;
}
