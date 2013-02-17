// board.cc - board representation and interface

#include "board.h"

#include <cstring>
#include <algorithm>

#include <glog/logging.h>

namespace chessy {

Board::Board() : color_(kWhite),
                 friends_(kInitialFriends),
                 enemies_(kInitialEnemies) {
  memcpy(reinterpret_cast<void *>(squares_),
         reinterpret_cast<const void *>(kInitialSquares),
         sizeof(squares_));
}

Board::Board(const Board& old, const Bitmove& move) {
  memcpy((void*)this, (void*)&old, sizeof(Board));
  Piece source_tile = squares_[move.source];
  Piece dest_tile = squares_[move.dest];
  DCHECK(move.source.IsValid()) << move;
  DCHECK(move.dest.IsValid()) << move;
  DCHECK(source_tile.piece() != kEmpty) << source_tile;
  DCHECK(source_tile.color() == color_) << source_tile;
  DCHECK(IsLegal(move, false)) << move;
  if (source_tile.piece() == kKing) {
    my_king_ = move.dest;
  }
  if (!dest_tile.IsEmpty()) {
    if (dest_tile.color() == color_) {
      LOG(INFO) << *this << move;
    }
    DCHECK(dest_tile.color() != color_);
    their_lost_ += dest_tile.value();
    enemies_ ^= move.dest_bit;
  }
  squares_[move.dest] = squares_[move.source];
  squares_[move.source] = Piece();
  friends_ ^= move.source_bit;
  friends_ |= move.dest_bit;
  color_ = Toggle(color_);
  std::swap(my_lost_, their_lost_);
  std::swap(friends_, enemies_);
  std::swap(my_king_, their_king_);
}

bool Board::operator==(const Board& other) const {
  return 0 == memcmp((void*)squares_, (void*)other.squares_, sizeof(squares_));
}

bool Board::IsChecking() const {
  Bitboard king(their_king_);
  for (int rank = 0; rank < kRow; ++rank) {
    for (int file = 0; file < kRow; ++file) {
      Square source(rank, file);
      Piece piece = squares_[source];
      if (piece.IsEmpty() || piece.color() != color_)
        continue;
      const Bitmove& move = GetBitmove(piece, source, their_king_);
      if ((king & move.dest_bit) && IsLegal(move, false)) {
        return true;
      }
    }
  }
  return false;
}

bool Board::IsLegal(const Bitmove& move, bool check_check) const {
  DCHECK(move.IsValid());
  DCHECK(move.path);
  Piece from = squares_[move.source];
  Piece to = squares_[move.dest];
  // Is dest a friend? Will I bump into any friends along the way?
  if (move.path & friends_) {
    VLOG(2) << from << " " << move << " is friend blocked\n\n"
            << move.path << "\n"
            << friends_;
    return false;
  }
  // Do any squares on the way to dest contain enemies blocking us?
  if ((move.path ^ move.dest_bit) & enemies_) {
    VLOG(2) << from << " " << move << " is blocked by enemies";
    return false;
  }
  DCHECK(from.color() == color_);
  DCHECK(from.piece() != kEmpty);
  // Pawns can only change file when attacking.
  if (from.piece() == kPawn && to.piece() == kEmpty &&
      move.source.file() != move.dest.file()) {
    VLOG(2) << from << " " << move << " pawn not attacking";
    return false;
  }
  // Does this move put me in check?
  if (check_check && Board(*this, move).IsChecking())
    return false;
  return true;
}

Bitmoves Board::PossibleMoves() const {
  Bitmoves res;
  for (int rank = 0; rank < kRow; ++rank) {
    for (int file = 0; file < kRow; ++file) {
      Square source(rank, file);
      Piece piece = squares_[source];
      if (piece.IsEmpty() || piece.color() != color_)
        continue;
      for (const Bitmove& move : GetBitmoves(piece, source)) {
        if (IsLegal(move, true)) {
          res.push_back(move);
        }
      }
    }
  }
  return res;
}

const Bitmove& Board::ComposeMove(Square source, Square dest) const {
  DCHECK(source.IsValid());
  DCHECK(dest.IsValid());
  Piece from = squares_[source];
  if (from.IsEmpty()) {
    VLOG(2) << source << " is an empty square";
    return Bitmove::kInvalid;
  }
  const Bitmove& move = GetBitmove(from, source, dest);
  if (!move.IsValid()) {
    VLOG(2) << from << " " << source  << dest << " not a valid move";
    return Bitmove::kInvalid;
  }
  if (!IsLegal(move, true)) {
    return Bitmove::kInvalid;
  }
  return move;
}

const Piece Board::kInitialSquares[128] = {
  [Square(0, 0)] = Piece(kWhite, kRook),
  [Square(0, 1)] = Piece(kWhite, kKnight),
  [Square(0, 2)] = Piece(kWhite, kBishop),
  [Square(0, 3)] = Piece(kWhite, kQueen),
  [Square(0, 4)] = Piece(kWhite, kKing),
  [Square(0, 5)] = Piece(kWhite, kBishop),
  [Square(0, 6)] = Piece(kWhite, kKnight),
  [Square(0, 7)] = Piece(kWhite, kRook),
  [Square(1, 0)] = Piece(kWhite, kPawn),
  [Square(1, 1)] = Piece(kWhite, kPawn),
  [Square(1, 2)] = Piece(kWhite, kPawn),
  [Square(1, 3)] = Piece(kWhite, kPawn),
  [Square(1, 4)] = Piece(kWhite, kPawn),
  [Square(1, 5)] = Piece(kWhite, kPawn),
  [Square(1, 6)] = Piece(kWhite, kPawn),
  [Square(1, 7)] = Piece(kWhite, kPawn),
  [Square(7, 0)] = Piece(kBlack, kRook),
  [Square(7, 1)] = Piece(kBlack, kKnight),
  [Square(7, 2)] = Piece(kBlack, kBishop),
  [Square(7, 3)] = Piece(kBlack, kQueen),
  [Square(7, 4)] = Piece(kBlack, kKing),
  [Square(7, 5)] = Piece(kBlack, kBishop),
  [Square(7, 6)] = Piece(kBlack, kKnight),
  [Square(7, 7)] = Piece(kBlack, kRook),
  [Square(6, 0)] = Piece(kBlack, kPawn),
  [Square(6, 1)] = Piece(kBlack, kPawn),
  [Square(6, 2)] = Piece(kBlack, kPawn),
  [Square(6, 3)] = Piece(kBlack, kPawn),
  [Square(6, 4)] = Piece(kBlack, kPawn),
  [Square(6, 5)] = Piece(kBlack, kPawn),
  [Square(6, 6)] = Piece(kBlack, kPawn),
  [Square(6, 7)] = Piece(kBlack, kPawn),
};

const Bitboard Board::kInitialFriends = Bitboard(0x000000000000ffff);
const Bitboard Board::kInitialEnemies = Bitboard(0xffff000000000000);

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os, true);
  return os;
}

}  // namespace chessy
