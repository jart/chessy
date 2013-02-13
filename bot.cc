// bot.cc - primary chessy 'AI' logic implementation
// 2013.02.09

#include <string>
#include <ctime>

#include "bot.h"
#include "render.h"

using std::cout;
using std::endl;

namespace chessy {
namespace bot {

  int Seed(Board* board) {
    return -operandi::NegaMax(board, kMaxDepth, kMinScore, kMaxScore);
  }

  void Chillax(int ms) {
    struct timespec req = {0};
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000L;
    fflush(stdout);   // ensure renders up-to-date
    nanosleep(&req, NULL);
  }

  void Think() {
    // TODO: meow vigorously
  }


namespace operandi { 

  // Maximizes the negation of the enemy player's positions
  int NegaMax(Board* board, int depth, int alpha, int beta) {

    Moves moves = board->PossibleMoves();
    dbg::BranchWindow(board, depth, moves.size(), alpha, beta);
    if (depth == 0 || moves.size() == 0) {
      int score = board->Score();
      dbg::Heuristic(board, score);
      // TODO: Quiescent search if last_move_ is "exciting"
      return score;
    }

    dbg::NewBranches(moves.size());
    for (const auto& move : moves) {

      board->Update(move);
      int val = -NegaMax(board, depth - 1, -beta, -alpha);
      board->Undo(move);

      // Beta pruning skips remaining branches, because the current sub-tree is 
      // now guaranteed to be futile (at least within the current depth)
      if (val >= beta) {
        dbg::BetaPrune(board, depth, val, moves.size());
        return val;
      }
      // Alpha just maximizes the negation of the next moves
      if (val >= alpha) {
        alpha = val;
      }
    }
    dbg::Negamaxed(board, depth, alpha);
    return alpha;
  }

}  // operandi
}  // bot



int g_dbg = 1;        // TODO: turn into a gflag
// ----------------------------- DEBUGGING -----------------------------
namespace dbg {
  int g_branches_searched = 0;
  int g_branches_pruned = 0;

  // Different debug levels
  enum Level {
    kNone = 0,
    kBasics = 1,
    kTree = 2,        // Dig into and step through the tree search
  };
  // ------------------- [1+] search metrics ---------------------
  // primary bot search
  void ChessyBeginsThinking(Board* board, int branches) {

    // BLACK chessy msgs on top, WHITE chessy msgs on bottom.
    if (kBlack == board->color())
      render::ChessyNewMsg("BLACK THOUGHTS~");
    else
      render::ChessyNewMsgMirror("WHITE THOUGHTS~");

    if (g_dbg < 1)
      return;

    render::ChessyMsg(                  // Basic metrics
        "  |d|="    + i2s(bot::kMaxDepth) + 
        "  |M|="    + i2s(branches) +
        "  |eval|=" + i2s(board->Score()) + 
        "\n\t" + term::kPink + u8"\u00A7");    // Begin progress bar
  }


  void NewBest(int score, const Move& move) {
    if (g_dbg < 1)  return; 
    string hval = (score >= 0 ?             // GREEN+ RED-
                   term::kGreen + "+" :
                   term::kRed) + i2s(score);
    render::ChessyMsg("{" + hval + term::kPink + "}");

    // Further |move| detail
    if (g_dbg < 2)  return;
    render::ChessyMsg(
        "Best move thus far: " + PrintMove(move) +
        " (s-val=" + i2s(score) + ")\n\t");
  }
  void ChessyProgress() {
    if (kHuman == g_mode)   // slow down for stupid humans
      bot::Chillax(1);
    Step(2);
    if (g_dbg < 1)
      return;
    render::ChessyMsg(u8"\u00BB");  // Indicator
    fflush(stdout);
  }

  void ChessyFinishesThinking(int score) {
    if (g_dbg < 1)
      return;

    // chessy has happy metrics!
    float savings = static_cast<float>(g_branches_pruned) /
                    static_cast<float>(g_branches_searched) * 100;
    string happy = score >= 0 ? 
        term::kGreen + "Q(^_^ Q)":
        term::kRed   + u8"\u00F7(-_- \u00F7)";

    render::ChessyMsg(
        u8"\u00A7 " + happy + term::kPink +
        "\n\t\t total branches: " + i2s(g_branches_searched) +
        "\n\t\t total pruned:   " + i2s(g_branches_pruned) +
        "\n\t\t search savings: " + i2s(savings) + "%");
  }

  // ------------------ [2+] gametree step-through ----------------------

  //  negamax alpha-beta pruning
  void Negamaxed(Board* board, int depth, int alpha) {
    if (g_dbg < 2)
      return;

    DepthIndent(depth);
    cout << "<--- a-negamaxed(" << board->color_str() << ")=" << alpha;
  }
  void BetaPrune(Board* board, int depth, int beta, int pruned) {
    g_branches_pruned += pruned;
    if (g_dbg < 2)
      return;
    DepthIndent(depth);
    cout << "<-- b-pruned(" << board->color_str() << ")=" << beta;
  }
  void NewBranches(int increment) {
    g_branches_searched += increment;
  }
  void BranchWindow(Board* board, int depth, int branches, int alpha, int beta) {
    if (g_dbg < 2)
      return;
    DepthIndent(depth);

    const auto& last_move = board->last_move();
    cout << "" << branches << "-< ("
         << ColorString(Toggle(board->color())) << " " << last_move 
         << ") a[" << alpha
         << "] b[" << beta << "] >- ";
  }
  void Heuristic(Board* board, int score) {
    if (g_dbg < 2)
      return;

    // Inline with BranchWindow, when applicable (leaf nodes)
    cout << "h-val(" << board->color_str() << ")=" << score;
  }

  string _lol;
  void Step(int min_level) {
    if (g_dbg < min_level)
      return;

    // Pause for keypresses
    std::getline(std::cin, _lol);
  }
  void DepthIndent(int depth) {
    cout << endl;
    for (int i = bot::kMaxDepth - depth ; i > 0 ; --i) {
      cout << " + ";
    }
  }
}  // dbg
}  // chessy
