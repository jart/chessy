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

using std::array;
using std::bitset;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace chessy {

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os);
  return os;
}

int NegaMax(Board* board, int depth, int alpha, int beta, Move* o_best) {
  vector<Move> moves = board->PossibleMoves();
  if (depth == 0 || moves.size() == 0) {
    int score = board->Score() * ((board->color() != kWhite) ? 1 : -1);
    // cout << "SCORE=" << score << endl;
    return score;
  }

  while (moves.size() > 0) {
    const Move move = GetMove(&moves);
    board->Update(move);

    /*
    cout << *board
         << endl
         << "alpha=" << alpha
         << " beta=" << beta
         << " depth=" << depth
         << " moves=" << moves.size()
         << endl;
    string lol;
    std::getline(std::cin, lol);
    */

    int val = -NegaMax(board, depth - 1, -beta, -alpha, NULL);
    board->Undo(move);
    if (val >= beta) {
      return val;
    }
    if (val >= alpha) {
      alpha = val;
      if (o_best) {
        *o_best = move;
      }
    }
  }
  return alpha;
}

Move BestMove(Board* board) {
  Move best;
  NegaMax(board, 5, -9999, +9999, &best);
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

std::ostream& operator<<(std::ostream& os, const Square& square) {
  if (square < 0 || square >= kSquares) {
    os << "NA(" << static_cast<int>(square) << ")";
    return os;
  }
  string algebraic;
  algebraic += 'a' + File(square);
  algebraic += '1' + Rank(square);
  os << algebraic;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
  // os << "[" << move.type << move.captured << "] "
  os << move.source << " -> " << move.dest;
  return os;
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
  return ((a.type == b.type)
          && (a.captured == b.captured)
          && (a.source == b.source)
          && (a.dest == b.dest));
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

    if (toupper(option[0]) == 'Y') {
      g_mode = kHuman;
      cout << "You are WHITE and Chessy is BLACK." << endl;
    } else {
      cout << "Chessy is playing Chessy!" << endl;
    }

    chessy::g_state = kPlaying;
    while(kPlaying == g_state) {
      cout << board << endl;
      Move move = kInvalidMove;
      if (g_mode == kHuman && kWhite == board.color()) {
        move = HumanMove(&board);
        assert(kInvalid != move.type);
        cout << "You moved: ";
      } else {
        move = BestMove(&board);
        cout << "Chessy: ";
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
