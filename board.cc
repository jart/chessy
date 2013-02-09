// board.c - board implementations
// 2013.02.08

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>

#include "board.h"

// using std::array;
using std::cout;
using std::endl;

namespace chessy {

inline int Rank(Square square) {
  return square / kRow;
}

inline int File(Square square) {
  return square % kRow;
}

inline Color Toggle(Color color) {
  return (color == kWhite) ? kBlack : kWhite;
}

inline int PieceIndex(Color color, Piece piece) {
  return piece + (color == kWhite? 0 : kPieceTypes);
}

Board::Board() : board_(kInitialBoardPosition), color_(kWhite) {

  // Sync the SquareTable and PieceTable according to BitBoards
  for (int square = 0; square < kSquares; ++square) {
    square_table_[square] = SquareState(square);
    for (int piece = kPawn; piece < kPieceTypes; ++piece) {
      Piece sc_piece = static_cast<Piece>(piece);
      if (GetBitBoard(kWhite, sc_piece)[square]) {
        square_table_[square] = SquareState(kWhite, sc_piece, square);
        break;
      }
      if (GetBitBoard(kBlack, sc_piece)[square]) {
        square_table_[square] = SquareState(kBlack, sc_piece, square);
        break;
      }
    }
  }
}

void Board::Update(const Move& move) {
  assert(move.type != kInvalid);
  // if (move.captured == kKing) {
  //   switch (color_) {
  //     case kWhite:
  //       cout << "White wins!" << endl;
  //       exit(0);
  //     case kBlack:
  //       cout << "Black wins!" << endl;
  //       exit(0);
  //   }
  // }
  auto& source = square_table_[move.source];
  auto& dest = square_table_[move.dest];
  assert(!source.empty);

  source.empty = true;
  dest.empty = false;
  dest.piece = source.piece;
  dest.color = source.color;

  color_ = Toggle(color_);

  // Flush memoized bit masks
  friends_ = kEmpty;
  enemies_ = kEmpty;
  last_move_ = move;
}

void Board::Undo(const Move& move) {
  assert(move.type != kInvalid);
  auto& source = square_table_[move.source];
  auto& dest = square_table_[move.dest];
  assert(!dest.empty);

  source.empty = false;
  source.piece = dest.piece;

  if (move.type == kAttack) {
    dest.empty = false;
    dest.color = color_;
    dest.piece = move.captured;
  } else {
    dest.empty = true;
  }
 
  color_ = Toggle(color_);
  source.color = color_;

  // Flush memoized bit masks
  friends_ = kEmpty;
  enemies_ = kEmpty;
}

vector<Move> Board::PossibleMoves() const {
  vector<Move> res;
  for (const auto& state : square_table_) {
    if (state.empty || state.color != color_) {
      continue;
    }
    switch (state.piece) {
      case kPawn:
        PawnMoves(&res, state.square);
        break;
      case kKnight:
        KnightMoves(&res, state.square);
        break;
      case kBishop:
        BishopMoves(&res, state.square);
        break;
      case kRook:
        RookMoves(&res, state.square);
        break;
      case kQueen:
        QueenMoves(&res, state.square);
        break;
      case kKing:
        KingMoves(&res, state.square);
        break;
    }
  }

  return res;
}

MoveType Board::QualifySquare(Square square) {
  if (Friends()[square]) {
    return kInvalid;
  }
  if (Enemies()[square]) {
    return kAttack;
  }
  return kRegular;
}

Move Board::ComposeMove(Square source, Square dest) const {
  const auto& state = square_table_[dest];
  if (state.empty) {
    return Move(kRegular, source, dest);
  }
  if (state.color == color_) {
    return Move(kInvalid, source, dest);
  }
  return Move(kAttack, source, dest, state.piece);
  // TODO:  King conditions and Queening
}

Move Board::TryMove(Square source, Delta delta) const {
  // Check boundaries
  Offset rank = Rank(source) + delta.rank;
  Offset file = File(source) + delta.file;
  if ((rank < 0 || rank >= kRow) || 
     (file < 0 || file >= kRow)) {
    return kInvalidMove;
  }
  Square dest = rank * kRow + file;
  return ComposeMove(source, dest);
}

void Board::PawnMoves(vector<Move>* res, Square source) const {
  Delta forward = (color_ == kWhite ? kU : kD);
  Move move;
  move = TryMove(source, forward + kR);
  if (move.type == kAttack) {
    AddMove(res, move);
  }
  move = TryMove(source, forward + kL);
  if (move.type == kAttack) {
    AddMove(res, move);
  }
  move = TryMove(source, forward);
  if (move.type == kRegular) {
    AddMove(res, move);
    if ((color_ == kWhite && Rank(source) == 1) ||
        (color_ == kBlack && Rank(source) == 6)) {
      move = TryMove(source, forward * 2);
      if (move.type == kRegular) {
        AddMove(res, move);
      }
    }
  }
  // TODO: Promotion
  // TODO: En Passant
}

void Board::KnightMoves(vector<Move>* res, Square source) const {
  for (const auto delta : kKnightDeltas) {
    Move move = TryMove(source, delta);
    if (move.type != kInvalid) {
      AddMove(res, move);
    }
  }
}

void Board::SlidingMoves(vector<Move>* res, Square source, Delta d) const {
  Delta dest = d;
  Move move(kRegular);
  while (move.type == kRegular) {
    move = TryMove(source, dest);
    if (move.type != kInvalid) {
      AddMove(res, move);
    }
  dest += d;
  }
}

void Board::BishopMoves(vector<Move>* res, Square source) const {
  for (const auto delta : kDiagonals) {
    SlidingMoves(res, source,  delta);
  }
}

void Board::RookMoves(vector<Move>* res, Square source) const {
  for (const auto delta : kOrthogonals) {
    SlidingMoves(res, source, delta);
  }
}

void Board::QueenMoves(vector<Move>* res, Square source) const {
  BishopMoves(res, source);
  RookMoves(res, source);
}

void Board::KingMoves(vector<Move>* res, Square source) const {
  for (const auto delta : kOmnigonals) {
    Move move = TryMove(source, delta);
    if (move.type != kInvalid) {
      AddMove(res, move);
    }
  }
}

int Board::Score() const {
  int us = 0;
  int them = 0;
  for (const auto& state : square_table_) {
    if (state.empty) {
      continue;
    }
    if (color_ == state.color) {
      us += kPieceValue[state.piece];
    } else {
      them += kPieceValue[state.piece];
    }
  }
  return us - them;
}

BitBoard Board::GetBitBoard(Color color, Piece piece) const {
  return board_[PieceIndex(color, piece)];
}

BitBoard Board::Friends() {
  if (kEmpty == friends_)
    friends_ = PositionMask(color_);
  return friends_;
}

BitBoard Board::Enemies() {
  if (kEmpty == enemies_)
    enemies_ = PositionMask(Toggle(color_));
  return enemies_;
}

BitBoard Board::PositionMask(Color color) const {
  BitBoard mask = kEmpty;
  for (int piece = 0 ; piece < kPieceTypes ; ++piece) {
    mask |= GetBitBoard(color, static_cast<Piece>(piece));
  }
  return mask;
}

Piece Board::PieceAt(Square square) const {
  const auto& state = square_table_[square];
  if (state.empty) {
    return kNoPiece;
  }
  return state.piece;
}

Color Board::ColorAt(Square square) const {
  return square_table_[square].color;
}


}  // chessy
