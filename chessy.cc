// chessy - a chess engine by justine and serene
// February 3rd, 2013

#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <vector>

#include "chessy.h"
#include "board.h"
#include "bot.h"
#include "render.h"


using std::array;
using std::bitset;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace chessy {

GameMode g_mode = kMirror;
GameState g_state = kNone;
int g_dbg = 1;
int g_nodes_searched = 0;
int g_nodes_pruned = 0;

int NegaMax(Board* board, int depth, int alpha, int beta) {
  vector<Move> moves = board->PossibleMoves();
  // Debug with branch factor and window
  if (g_dbg > 1) {
    DepthCout(depth);
    cout << "" << moves.size() << "-< ("
         << ColorString(Toggle(board->color())) << " " << board->last_move()
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

  while (moves.size() > 0) {
    const Move move = GetMove(&moves);
    board->Update(move);

    int val = -NegaMax(board, depth - 1, -beta, -alpha);
    board->Undo(move);

    // Beta pruning skips remaining branches, because the current sub-truee is 
    // now guaranteed to offer no improvement.
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

  vector<Move> moves = board->PossibleMoves();
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

    if (g_dbg > 1) {
      std::getline(std::cin, lol);
    }
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

bool CompareMoves(const Move& a, const Move& b) {
  if (a.score == b.score) {
    return std::rand() % 2 == 0;
  } else {
    return a.score > b.score;
  }
}

void AddMove(vector<Move>* moves, const Move& move) {
//  cout << "Adding move " << move << endl;
  moves->push_back(move);
  std::push_heap(moves->begin(), moves->end(), CompareMoves);
}

// TODO: Turn into transposition table
Move GetMove(vector<Move>* moves) {
  std::pop_heap(moves->begin(), moves->end(), CompareMoves);
  Move move = moves->back();
  moves->pop_back();
  return move;
}

Move ValidateAlgebraicMove(const string& input) {
  if (input.size() != 4)
    return kInvalidMove;

  Offset file = input[0] - 'a';
  if (file < 0 || file >= kRow)
    return kInvalidMove;
  Offset rank = input[1] - '1';
  if (rank < 0 || rank >= kRow)
    return kInvalidMove;
  Square source = rank * kRow + file;

  file = input[2] - 'a';
  if (file < 0 || file >= kRow)
    return kInvalidMove;
  rank = input[3] - '1';
  if (rank < 0 || rank >= kRow)
    return kInvalidMove;
  Square dest = rank * kRow + file;

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
        valid_move = true;
        return move;
      }
    }
    cout << move << " is invalid for the "
         << "current board position." << endl;
  }
  return kInvalidMove;
}

void GameLoop() {
  cout << "Chessy v0.1" << endl
       << "A chess bot by Justine and Serene ^_^"
       << endl << endl;

  Board board;

  while(true) {
    cout << "Would you like to play against Chessy? [y/n]" << endl;

    string option;
    std::getline(std::cin, option);

    if (std::toupper(option[0]) == 'Y') {
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
    }
  }

}

} // chessy


int main(int argc, char** argv) {
  std::srand(static_cast<unsigned>(std::time(0)));
  chessy::GameLoop();
  return 0;
}
