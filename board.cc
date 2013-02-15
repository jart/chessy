// board.c - board implementations
// 2013.02.08

#include "board.h"

#include <cstdlib>
#include <iostream>

#include <glog/logging.h>

#include "chessy.h"
#include "move.h"
#include "render.h"
#include "square.h"

using std::cout;
using std::endl;

namespace chessy {

const BoardPosition kInitialBoardPosition = {{
  BitBoard("11111111") << kRow,        // White Pawn
  BitBoard("01000010"),                // White Knight
  BitBoard("00100100"),                // White Bishop
  BitBoard("10000001"),                // White Rook
  BitBoard("00001000"),                // White Queen
  BitBoard("00010000"),                // White King
  BitBoard("11111111") << (kRow * 6),  // Black Pawn
  BitBoard("01000010") << (kRow * 7),  // Black Knight
  BitBoard("00100100") << (kRow * 7),  // Black Bishop
  BitBoard("10000001") << (kRow * 7),  // Black Rook
  BitBoard("00001000") << (kRow * 7),  // Black Queen
  BitBoard("00010000") << (kRow * 7),  // Black King
}};

Board::Board() : board_(kInitialBoardPosition), color_(kWhite) {
  Reset();
}

void Board::Reset() {
  // Sync the SquareTable and PieceTable according to BitBoards.
  board_ = kInitialBoardPosition;
  color_ = kWhite;
  for (int square = 0; square < kTotal; ++square) {
    square_table_[square].index = square;
    square_table_[square].empty = true;
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
  king_square_[kWhite] = 4;
  king_square_[kBlack] = 60;
}

void Board::Update(const Move& move) {
  CHECK(kInvalidMove != move.type);
  CHECK(kTentative != move.type);

  VLOG(3) << "Board update: " << move;

  // Null Moves are like "passing", which is invalid in real chess but can be
  // abused for our gametree.
  if (kNullMove == move.type) {
    color_ = Toggle(color_);
    friends_ ^= enemies_ ^= friends_ ^= enemies_;
    attacked_ = kEmptyBoard;
    return;
  }

  auto& source = square_table_[Index(move.source)];
  auto& dest = square_table_[Index(move.dest)];
  CHECK(!source.empty);

  // Update king index
  // TODO: Implement PieceTable (which generalizes this) for faster lookups
  //if (kKing == source.piece) {
  //  king_square_[color_] = Getx88(dest.index);
  //}

  source.empty = true;
  dest.empty = false;
  dest.piece = source.piece;
  dest.color = source.color;

  color_ = Toggle(color_);

  // Flush memoized bit masks
  friends_ = kEmptyBoard;
  enemies_ = kEmptyBoard;
  attacked_ = kEmptyBoard;

  // TODO: Transposition table which restores these masks

  last_move_ = move;
}

void Board::Undo(const Move& move) {
  //CHECK(last_move_ == move);
  CHECK(move.type != kInvalidMove);
  auto& source = square_table_[Index(move.source)];
  auto& dest = square_table_[Index(move.dest)];

  VLOG(3) << "Board undo: " << move;

  if (kNullMove == move.type) {
    color_ = Toggle(color_);
    friends_ ^= enemies_ ^= friends_ ^= enemies_;
    attacked_ = kEmptyBoard;
    return;
  }
  CHECK(!dest.empty);

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

  // Update king index
  //if (kKing == source.piece) {
  //  king_square_[color_] = Getx88(dest.index);
  //}

  // Flush memoized bit masks
  friends_ = kEmptyBoard;
  enemies_ = kEmptyBoard;
  attacked_ = kEmptyBoard;
  // TODO: Transposition table which restores these masks
}

Moves Board::PossibleMoves() {
  Moves res;
  for (const auto& state : square_table_) {
    if (state.empty || state.color != color_)
      continue;
    Squares targets = Squares();
    Square square = Getx88(state.index);
    switch (state.piece) {
      case kPawn:
        targets = PawnTargets(square);
        break;
      case kKnight:
        targets = KnightTargets(square);
        break;
      case kBishop:
        targets = BishopTargets(square);
        break;
      case kRook:
        targets = RookTargets(square);
        break;
      case kQueen:
        targets = QueenTargets(square);
        break;
      case kKing:
        targets = KingTargets(square);
        break;
      default:
        CHECK(false) << state.piece;
    }
    for (Square dest : targets) {
      AddMove(&res, ComposeMove(square, dest));
    }
  }
  // Flatten move heap into list
  return res;
}

MoveType Board::QualifyTarget(Square target) {
  if (!Valid(target))
    return kInvalidMove;
  int index = Index(target);
  const auto& state = square_table_[index];
  if (state.empty)
    return kRegular;
  if (state.color == color_)
    return kInvalidMove;
  return kAttack;
}

Move Board::ComposeMove(Square source, Square dest) {
  const auto& state = square_table_[Index(dest)];
  MoveType type = QualifyTarget(dest);
  if (kInvalidMove == type ||
      (kPawn == PieceAt(source) &&
       kAttack == type &&
       File(source) == File(dest))) {
    return Move(kInvalidMove, source, dest);
  }
  if (kAttack == type) {
    return Move(kAttack, source, dest, state.piece);
  }
  return Move(kRegular, source, dest);
  // TODO: King conditions and queening.
}

Squares Board::PawnTargets(const Square source) {
  Squares res;

  // Pawns are the only piece with directional limitation.
  Offset forward = (color_ == kWhite ? delta::kU : delta::kD);
  Square front = source + forward;
  Square diagR = front + delta::kR;
  Square diagL = front + delta::kL;

  // Check diagonal attacks
  if (kAttack == QualifyTarget(diagR))
    res.emplace_front(diagR);

  if (kAttack == QualifyTarget(diagL))
    res.emplace_front(diagL);

  // Forward moves (no attack)
  if (kRegular == QualifyTarget(front)) {
    res.emplace_front(front);

    // If it's the pawn's first move, check for an advance
    if ((color_ == kWhite && Rank(source) == 1) ||
        (color_ == kBlack && Rank(source) == 6)) {

      Square advance = front + forward;
      if (kRegular == QualifyTarget(advance)) {
        res.emplace_front(advance);

        // TODO: Set En Passant Square at |front| (which is now behind)
      }
    }
  }

  // TODO: Promotion
  // TODO: En Passant

  return res;
}

Squares Board::KnightTargets(Square source) {
  Squares res;
  for (const auto delta : delta::kKnight) {
    Square target = source + delta;
    if (kInvalidMove != QualifyTarget(target)) {
      res.emplace_front(target);
    }
  }
  return res;
}

Squares Board::BishopTargets(Square source) {
  Squares res;
  for (const auto delta : delta::kDiagonal) {
    SlidingTargets(&res, source,  delta);
  }
  return res;
}

Squares Board::RookTargets(Square source) {
  Squares res;
  for (const auto delta : delta::kOrthogonal) {
    SlidingTargets(&res, source, delta);
  }
  return res;
}

Squares Board::QueenTargets(Square source) {
  Squares res;
  for (const auto delta : delta::kOmnigonal) {
    SlidingTargets(&res, source, delta);
  }
  return res;
  /*    Squares res;
        Squares diagonals = BishopTargets(source);
        Squares orthogonals = RookTargets(source);
        res.splice_after(res.cend(), diagonals, diagonals.cbegin());
        res.splice_after(res.cend(), orthogonals, orthogonals.cbegin());
        return res; */
}

Squares Board::KingTargets(Square source) {
  Squares res;
  for (const auto delta : delta::kOmnigonal) {
    Square target = source + delta;
    if (kInvalidMove != QualifyTarget(target)) {
      res.emplace_front(target);
    }
  }
  return res;
}

void Board::SlidingTargets(Squares* res, Square source, Offset delta) {
  Square dest = source + delta;
  MoveType type = kRegular;
  while (kRegular == type) {
    type = QualifyTarget(dest);
    if (kInvalidMove != type) {
      res->emplace_front(dest);
    }
    dest += delta;
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
      us += kPieceValues[state.piece];
    } else {
      them += kPieceValues[state.piece];
    }
  }
  return us - them;
}

BitBoard Board::Friends() {
  if (kEmptyBoard == friends_) {
    friends_ = PositionMask(color_);
  }
  return friends_;
}

BitBoard Board::Enemies() {
  if (kEmptyBoard == enemies_) {
    enemies_ = PositionMask(Toggle(color_));
  }
  return enemies_;
}

BitBoard Board::PositionMask(Color color) const {
  BitBoard mask = kEmptyBoard;
  for (int piece = 0; piece < kPieceTypes; ++piece) {
    mask |= GetBitBoard(color, static_cast<Piece>(piece));
  }
  return mask;
}

BitBoard Board::GetBitBoard(Color color, Piece piece) const {
  return board_[PieceIndex(color, piece)];
}

Piece Board::PieceAt(Square square) const {
  const auto& state = square_table_[Index(square)];
  if (state.empty) {
    return kNoPiece;
  }
  return state.piece;
}

Color Board::ColorAt(Square square) const {
  return square_table_[Index(square)].color;
}

bool Board::ActiveSquare(const Square square) const {
  if (last_move_ == kBadMove)
    return false;
  return (square == last_move_.source ||
          square == last_move_.dest);
}

void Board::SetAttacked(Square square) {
  attacked_[square] = 1;
}

bool Board::SquareAttacked(Square square, Color color) const {
  if (color == color_) {
    return attacked_[square] == 1;
  }

  /*
    const auto& state = board->square_table_[square];
    if (kEmptyBoard == attacked_) {
    return false;
    }
    attacked_
  */
  return false;
}

bool ValidMove(Board* board, const Move move) {
  // Ensure that after this move, the board is not in check.
  bool res = true;
  Color color = board->color();
  board->Update(move);
  board->PossibleMoves();
  res = InCheck(board, color);
  board->Undo(move);
  return res;
}

Square Board::KingSquare(Color color) const {
  return king_square_[color];
}

//bool Board::InCheck() {
//  return InCheck(this, color_);
//}

bool InCheck(Board* board, Color color) {
  return false;
  // Is |color| king under attack?
  Square king = board->KingSquare(color);

  // Checking if enemy is in check
  if (color != board->color()) {
    return board->SquareAttacked(king, color);
  }

  // Checking if current player is in check (more expensive for now)
  bool res = false;
  Move nm(kNullMove);
  board->Update(nm);
  board->PossibleMoves();
  res = board->SquareAttacked(king, board->color());
  board->Undo(nm);
  return res;
}

bool Checkmate(Board* board) {
  return false;
  Color color = board->color();
  if (!InCheck(board, color)) {
    return false;
  }

  // Assume checkmate until proven innocent.
  bool res = true;
  Move nm(kNullMove);
  for (const auto& move : board->PossibleMoves()) {
    board->Update(move);
    board->Update(nm);

    CHECK(color == board->color());
    res = InCheck(board, color);

    board->Undo(nm);
    board->Undo(move);
    if (!res)
      return false;
  }

  // No available move gets out of Check.
  return true;
}

bool Stalemate(Board* board) {
  const auto& moves = board->PossibleMoves();
  // TODO: Repeated position stalemate checks
  return moves.size() == 0;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
  board.Print(os, true);
  return os;
}

}  // namespace chessy
