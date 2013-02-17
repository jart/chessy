// board.h - board representation and interface

#ifndef CHESSY_BOARD_H_
#define CHESSY_BOARD_H_

#include <functional>
#include <ostream>

#include "bitmove.h"

namespace chessy {

class Board {
 public:
  Board();
  Board(const Board& old, const Bitmove& move);
  Board(const Board& old) = delete;
  inline Colors color() const { return color_; }
  inline int score() const { return their_lost_ - my_lost_; }
  inline Piece GetPiece(Square square) const { return squares_[square]; }
  bool IsChecking() const;  // Are we putting the other player in check?
  size_t Hash() const { return (friends_ ^ enemies_).bits(); }
  bool operator==(const Board& other) const;
  void Print(std::ostream& os, bool redraw) const;
  Bitmoves PossibleMoves() const;
  bool IsLegal(const Bitmove& move, bool check_check) const;
  const Bitmove& ComposeMove(Square source, Square dest) const;

 private:
  static const Piece kInitialSquares[128];
  static const Bitboard kInitialFriends;
  static const Bitboard kInitialEnemies;

  Piece squares_[128];   // Maps Square to Piece. Canonical state of board.
  Colors color_;         // Current color playing the board (starts as kWhite).
  Bitboard friends_;     // Mask of pieces on our team.
  Bitboard enemies_;     // Mask of pieces on other team.
  Square my_king_;       // Where is current player's king?
  Square their_king_;    // Where is other player's king?
  int my_lost_;          // Sum of value of pieces lost by current player.
  int their_lost_;       // Sum of value of pieces lost by other player.
};

std::ostream& operator<<(std::ostream& os, const Board& board);

}  // namespace chessy

namespace std {

template<>
struct hash<chessy::Board> {
 public:
  size_t operator()(const chessy::Board& board) const {
    return board.Hash();
  }
};

}  // namespace std

#endif  // CHESSY_BOARD_H_
