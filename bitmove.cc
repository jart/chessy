// bitmove.cc

#include "bitmove.h"

#include <glog/logging.h>

namespace chessy {

const Bitmove Bitmove::kInvalid(Square::kInvalid);

// Stores all potential moves which can be made by a piece of a certain color
// at a certain position. The index for this vector is 11-bits which consists
// of 7 bits from class Square and 4 bits from class Piece.
static Bitmoves g_possible_moves[2048];

// A single bitboard mask of all the Bitmoves.
static Bitboard g_possible_mask[2048];

static inline size_t Index(Piece piece, Square square) {
  DCHECK(square.IsValid()) << square;
  size_t res = ((static_cast<size_t>(square.x88())) |
                (static_cast<size_t>(piece.bits()) << 7));
  DCHECK(0 <= res && res < 2048);
  return res;
}

const Bitmove& GetBitmove(Piece piece, Square source, Square dest) {
  for (const Bitmove& move : g_possible_moves[Index(piece, source)]) {
    if (dest == move.dest) {
      return move;
    }
  }
  return Bitmove::kInvalid;
}

const Bitmoves& GetBitmoves(Piece piece, Square source) {
  return g_possible_moves[Index(piece, source)];
}

const Bitboard& GetBitmovesMask(Piece piece, Square source) {
  return g_possible_mask[Index(piece, source)];
}

static void PawnMoves(Colors color, Square source) {
  size_t index = Index(Piece(color, kPawn), source);
  Square direction = (color == kWhite) ? Square::kUp : Square::kDown;
  const Square deltas[3] = { direction,
                             direction + Square::kLeft,
                             direction + Square::kRight };
  for (int n = 0; n < 3; n++) {
    Square dest = source + deltas[n];
    if (!dest.IsValid())
      continue;
    Bitboard path = Bitboard(dest);
    g_possible_moves[index].emplace_back(source, dest, path);
    g_possible_mask[index] |= Bitboard(dest);
  }
  // If starting rank, allow two piece move.
  if ((color == kWhite && source.rank() == 1) ||
      (color == kBlack && source.rank() == 6)) {
    Square dest = source + direction + direction;
    DCHECK(dest.IsValid());
    Bitboard path = Bitboard(dest);
    g_possible_moves[index].emplace_back(source, dest, path);
  }
}

static void KnightMoves(Colors color, Square source) {
  static const Square deltas[8] = {
    Square::kUp + Square::kUp + Square::kLeft,
    Square::kUp + Square::kUp + Square::kRight,
    Square::kDown + Square::kDown + Square::kLeft,
    Square::kDown + Square::kDown + Square::kRight,
    Square::kLeft + Square::kLeft + Square::kUp,
    Square::kLeft + Square::kLeft + Square::kDown,
    Square::kRight + Square::kRight + Square::kUp,
    Square::kRight + Square::kRight + Square::kDown,
  };
  size_t index = Index(Piece(color, kKnight), source);
  for (int n = 0; n < 8; n++) {
    Square dest = source + deltas[n];
    if (!dest.IsValid())
      continue;
    Bitboard path = Bitboard(dest);
    g_possible_moves[index].emplace_back(source, dest, path);
  }
}

static void KingMoves(Colors color, Square source) {
  static const Square deltas[8] = {
    Square::kUp,
    Square::kDown,
    Square::kLeft,
    Square::kRight,
    Square::kUp + Square::kLeft,
    Square::kUp + Square::kRight,
    Square::kDown + Square::kLeft,
    Square::kDown + Square::kRight,
  };
  size_t index = Index(Piece(color, kKing), source);
  for (int n = 0; n < 8; n++) {
    Square dest = source + deltas[n];
    if (!dest.IsValid())
      continue;
    Bitboard path = Bitboard(dest);
    g_possible_moves[index].emplace_back(source, dest, path);
  }
}

static void Seek(size_t index, Bitmove move, Square delta) {
  move.dest = move.dest + delta;
  if (!move.dest.IsValid())
    return;
  move.dest_bit = Bitboard(move.dest);
  move.path |= move.dest_bit;
  g_possible_moves[index].push_back(move);
  Seek(index, move, delta);
}

static void RookMoves(Colors color, Square source) {
  size_t index = Index(Piece(color, kRook), source);
  Seek(index, Bitmove(source), Square::kUp);
  Seek(index, Bitmove(source), Square::kDown);
  Seek(index, Bitmove(source), Square::kRight);
  Seek(index, Bitmove(source), Square::kLeft);
}

static void BishopMoves(Colors color, Square source) {
  size_t index = Index(Piece(color, kBishop), source);
  Seek(index, Bitmove(source), Square::kUp + Square::kLeft);
  Seek(index, Bitmove(source), Square::kUp + Square::kRight);
  Seek(index, Bitmove(source), Square::kDown + Square::kLeft);
  Seek(index, Bitmove(source), Square::kDown + Square::kRight);
}

static void QueenMoves(Colors color, Square source) {
  size_t index = Index(Piece(color, kQueen), source);
  Seek(index, Bitmove(source), Square::kUp);
  Seek(index, Bitmove(source), Square::kDown);
  Seek(index, Bitmove(source), Square::kRight);
  Seek(index, Bitmove(source), Square::kLeft);
  Seek(index, Bitmove(source), Square::kUp + Square::kLeft);
  Seek(index, Bitmove(source), Square::kUp + Square::kRight);
  Seek(index, Bitmove(source), Square::kDown + Square::kLeft);
  Seek(index, Bitmove(source), Square::kDown + Square::kRight);
}

void InitBitmoves() {
  for (int color = 0; color < kColors; ++color) {
    for (int piece = 0; piece < kPieces; ++piece) {
      for (int rank = 0; rank < kRow; ++rank) {
        for (int file = 0; file < kRow; ++file) {
          Square source(rank, file);
          switch (piece) {
            case kEmpty:
              break;
            case kPawn:
              PawnMoves((Colors)color, source);
              break;
            case kKnight:
              KnightMoves((Colors)color, source);
              break;
            case kBishop:
              BishopMoves((Colors)color, source);
              break;
            case kRook:
              RookMoves((Colors)color, source);
              break;
            case kQueen:
              QueenMoves((Colors)color, source);
              break;
            case kKing:
              KingMoves((Colors)color, source);
              break;
            default:
              CHECK(false) << piece;
          }
        }
      }
    }
  }
}

std::string Bitmove::ToString() const {
  std::string res;
  res += source.ToString();
  res += "->";
  res += dest.ToString();
  return res;
}

std::ostream& operator<<(std::ostream& os, const Bitmove& move) {
  os << move.ToString();
  return os;
}

}  // namespace chessy
