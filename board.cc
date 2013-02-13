// board.c - board implementations
// 2013.02.08

#include <cstdlib>
#include <cassert>
#include <iostream>

#include "board.h"
#include "constants.h"
#include "move.h"
#include "render.h"

using std::cout;
using std::endl;

namespace chessy {

  Board::Board() : board_(kInitialBoardPosition), color_(kWhite) {
    Reset();
  }

  void Board::Reset() {
    // Sync the SquareTable and PieceTable according to BitBoards
    board_ = kInitialBoardPosition;
    color_ = kWhite;
    for (int square = 0; square < square::kTotal; ++square) {
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
    assert(kInvalid   != move.type);
    assert(kTentative != move.type);

    if (g_dbg > 3)
      cout << "Board update: " << move << endl;

    // Null Moves are like "passing", which is invalid in real chess but can be 
    // abused for our gametree
    if (kNullMove == move.type) {
      color_ = Toggle(color_);    
      friends_ ^= enemies_ ^= friends_ ^= enemies_;
      attacked_ = kEmpty;
      return;
    }

    auto& source = square_table_[square::Index(move.source)];
    auto& dest = square_table_[square::Index(move.dest)];
    assert(!source.empty);

    // Update king index
    // TODO: Implement PieceTable (which generalizes this) for faster lookups
    //if (kKing == source.piece) {
    //  king_square_[color_] = square::Getx88(dest.index);
    //}

    source.empty = true;
    dest.empty = false;
    dest.piece = source.piece;
    dest.color = source.color;

    color_ = Toggle(color_);

    // Flush memoized bit masks
    friends_ = kEmpty;
    enemies_ = kEmpty;
    attacked_ = kEmpty;

    // TODO: Transposition table which restores these masks

    last_move_ = move;
  }

  void Board::Undo(const Move& move) {
    //assert(last_move_ == move);
    assert(move.type != kInvalid);
    auto& source = square_table_[square::Index(move.source)];
    auto& dest = square_table_[square::Index(move.dest)];

    if (g_dbg > 3)
      cout << "Board undo: " << move << endl;

    if (kNullMove == move.type) {
      color_ = Toggle(color_);    
      friends_ ^= enemies_ ^= friends_ ^= enemies_;
      attacked_ = kEmpty;
      return;
    }
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

    // Update king index
    //if (kKing == source.piece) {
    //  king_square_[color_] = square::Getx88(dest.index);
    //}

    // Flush memoized bit masks
    friends_ = kEmpty;
    enemies_ = kEmpty;
    attacked_ = kEmpty;
    // TODO: Transposition table which restores these masks
  }

  Moves Board::PossibleMoves() {
    Moves res;
    for (const auto& state : square_table_) {
      if (state.empty || state.color != color_)
        continue;
      Squares targets = Squares();
      Square square = square::Getx88(state.index);
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
          assert(false);
      }

      for (Square dest : targets) {
        AddMove(&res, ComposeMove(square, dest));
      }
    }
    // Flatten move heap into list
    return res;
  }


  // --- Move generation ---

  MoveType Board::QualifyTarget(Square target) {
    if (!square::Valid(target))
      return kInvalid;  // 0x88 off-the-board
  
    int index = square::Index(target);
    const auto& state = square_table_[index];

    if (state.empty)
      return kRegular;
    
    if (state.color == color_)
      return kInvalid;

    return kAttack;
    /*
    // Cannot move onto a square occupied by self 
    if (Friends()[index])
      return kInvalid;

    // Moving onto square occupid by enemy is an attack
    if (Enemies()[index])
      return kAttack;
    // Moving to an empty square is (usually) a regular move
    return kRegular;
    */
  }

  Move Board::ComposeMove(Square source, Square dest) {
    const auto& state = square_table_[square::Index(dest)];

    MoveType type = QualifyTarget(dest);
    if (kInvalid == type ||
        (kPawn == PieceAt(source) && kAttack == type && (square::File(source) == square::File(dest))))
      return Move(kInvalid, source, dest);

    if (kAttack == type)
      return Move(kAttack, source, dest, state.piece);

    return Move(kRegular, source, dest);

    // TODO:  King conditions and Queening
  }

  Squares Board::PawnTargets(const Square source) {
    Squares res;

    // Pawns are the only piece with directional limitation.
    Offset forward = (color_ == kWhite ? deltas::kU : deltas::kD);
    Square front = source + forward;
    Square diagR = front + deltas::kR;
    Square diagL = front + deltas::kL;

    // Check diagonal attacks
    if (kAttack == QualifyTarget(diagR))
      res.emplace_front(diagR);

    if (kAttack == QualifyTarget(diagL))
      res.emplace_front(diagL);

    // Forward moves (no attack)
    if (kRegular == QualifyTarget(front)) {
      res.emplace_front(front);

      // If it's the pawn's first move, check for an advance
      if ((color_ == kWhite && square::Rank(source) == 1) ||
          (color_ == kBlack && square::Rank(source) == 6)) {

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
    for (const auto delta : deltas::kKnight) {
      Square target = source + delta;
      if (kInvalid != QualifyTarget(target))
        res.emplace_front(target);
    }
    return res;
  }
  Squares Board::BishopTargets(Square source) {
    Squares res;
    for (const auto delta : deltas::kDiagonal) {
      SlidingTargets(&res, source,  delta);
    }
    return res;
  }
  Squares Board::RookTargets(Square source) {
    Squares res;
    for (const auto delta : deltas::kOrthogonal)
      SlidingTargets(&res, source, delta);
    return res;
  }
  Squares Board::QueenTargets(Square source) {
    Squares res;
    for (const auto delta : deltas::kOmnigonal)
      SlidingTargets(&res, source, delta);
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
    for (const auto delta : deltas::kOmnigonal) {
      Square target = source + delta;
      if (kInvalid != QualifyTarget(target))
        res.emplace_front(target);
    }
    return res;
  }
  void Board::SlidingTargets(Squares* res, Square source, Offset delta) {
    Square dest = source + delta;

    MoveType type = kRegular;
    while (kRegular == type) {
      type = QualifyTarget(dest);
      if (kInvalid != type)
        res->emplace_front(dest);
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

  // --- Bit board helpers ---
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

  BitBoard Board::GetBitBoard(Color color, Piece piece) const {
    return board_[PieceIndex(color, piece)];
  }
  Piece Board::PieceAt(Square square) const {
    const auto& state = square_table_[square::Index(square)];
    if (state.empty) {
      return kNoPiece;
    }
    return state.piece;
  }

  Color Board::ColorAt(Square square) const {
    return square_table_[square::Index(square)].color;
  }

  bool Board::ActiveSquare(const Square square) const {
    if (last_move_ == kInvalidMove)
      return false;
    return square == last_move_.source ||
           square == last_move_.dest;
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
    if (kEmpty == attacked_) {
      return false;
    }
    attacked_
    */
    return false;
  }

  bool ValidMove(Board* board, const Move move) {
    // Ensure that after this move, the board is not in check
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

    // Assume checkmate until proven innocent
    bool res = true;
    Move nm(kNullMove);
    for (const auto& move : board->PossibleMoves()) {
      board->Update(move);
      board->Update(nm);

      assert(color == board->color());
      res = InCheck(board, color);

      board->Undo(nm);
      board->Undo(move);
      if (!res)
        return false;
    }

    // No available move gets out of Check
    return true;
  }

  bool Stalemate(Board* board) {
    const auto& moves = board->PossibleMoves();
    // TODO: Repeated position stalemate checks
    return moves.size() == 0;
  }

}  // chessy
