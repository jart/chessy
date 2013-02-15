#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "board.h"

using namespace std;
using namespace chessy;

const uint64_t kBitRow[8] = {
  0x00000000000000ff,
  0x000000000000ff00,
  0x0000000000ff0000,
  0x00000000ff000000,
  0x000000ff00000000,
  0x0000ff0000000000,
  0x00ff000000000000,
  0xff00000000000000,
};

const uint64_t kBitColumn[8] = {
  0x0101010101010101,
  0x0202020202020202,
  0x0404040404040404,
  0x0808080808080808,
  0x1010101010101010,
  0x2020202020202020,
  0x4040404040404040,
  0x8080808080808080,
};

const uint64_t kBitSlash[15] = {
  0x0000000000000001,
  0x0000000000000102,
  0x0000000000010204,
  0x0000000001020408,
  0x0000000102040810,
  0x0000010204081020,
  0x0001020408102040,
  0x0102040810204080,
  0x0204081020408000,
  0x0408102040800000,
  0x0810204080000000,
  0x1020408000000000,
  0x2040800000000000,
  0x4080000000000000,
  0x8000000000000000,
};

const uint64_t kBitBackslash[15] = {
  0x0100000000000000,
  0x0201000000000000,
  0x0402010000000000,
  0x0804020100000000,
  0x1008040201000000,
  0x2010080402010000,
  0x4020100804020100,
  0x8040201008040201,
  0x0080402010080402,
  0x0000804020100804,
  0x0000008040201008,
  0x0000000080402010,
  0x0000000000804020,
  0x0000000000008040,
  0x0000000000000080,
};

constexpr uint64_t BitBit(int rank, int file) {
  return (rank < 8 && file < 8) ? 1ull << (rank * 8) << file : 0;
}

constexpr uint64_t BitRook(int rank, int file) {
  return kBitRow[rank] | kBitColumn[file];
}

constexpr uint64_t BitBishop(int rank, int file) {
  return kBitSlash[rank + file] | kBitBackslash[rank - file + 7];
}

constexpr uint64_t BitQueen(int rank, int file) {
  return BitRook(rank, file) | BitBishop(rank, file);
}

constexpr int Abs(int x) {
  return (x > 0) ? x : 0;
}

constexpr uint64_t BitKing(int rank, int file) {
  return ((file > 0)
          ? ((7 << (file - 1) << (Abs(rank - 1) * 8)) |
             (7 << (file - 1) << (rank * 8)) |
             (7 << (file - 1) << ((rank + 1) * 8)))
          : ((3 << (Abs(rank - 1) * 8)) |
             (3 << (rank * 8)) |
             (3 << ((rank + 1) * 8))));
}

constexpr uint64_t BitPawn(int rank, int file, Color color) {
  return (BitBit(rank, file) |
          ((color == kWhite)
           ? (BitBit(rank + 1, file - 1) | BitBit(rank + 1, file + 1))
           : (BitBit(rank - 1, file - 1) | BitBit(rank - 1, file + 1))));
}

constexpr uint64_t BitKnight(int rank, int file) {
  return (BitBit(rank, file) |
          BitBit(rank + 2, file - 1) |
          BitBit(rank + 2, file + 1) |
          BitBit(rank - 2, file + 1) |
          BitBit(rank - 2, file - 1) |
          BitBit(rank + 1, file + 2) |
          BitBit(rank + 1, file - 2) |
          BitBit(rank - 1, file + 2) |
          BitBit(rank - 1, file - 2));
}

constexpr size_t BitLocate(Color color, Piece piece, int rank, int file) {
  return ((static_cast<size_t>(file)  << 0) |  // 3 bits
          (static_cast<size_t>(rank)  << 3) |  // 3 bits
          (static_cast<size_t>(piece) << 6) |  // 3 bits
          (static_cast<size_t>(color) << 9));  // 2 bits
}

// for piece in ['Pawn', 'Knight', 'Bishop', 'Rook', 'Queen', 'King']:
//     for color in ['kWhite', 'kBlack']:
//         for rank in range(8):
//             for file_ in range(8):
//                 extra = ''
//                 if piece == 'Pawn':
//                     extra = ', ' + color
//                 print '  [BitLocate(%s, k%s, %d, %d)] = Bit%s(%d, %d%s),' % (
//                     color, piece, rank, file_, piece, rank, file_, extra)

static uint64_t BitStatic[] = {
  [BitLocate(kWhite, kPawn, 0, 0)] = BitPawn(0, 0, kWhite),
  [BitLocate(kWhite, kPawn, 0, 1)] = BitPawn(0, 1, kWhite),
  [BitLocate(kWhite, kPawn, 0, 2)] = BitPawn(0, 2, kWhite),
  [BitLocate(kWhite, kPawn, 0, 3)] = BitPawn(0, 3, kWhite),
  [BitLocate(kWhite, kPawn, 0, 4)] = BitPawn(0, 4, kWhite),
  [BitLocate(kWhite, kPawn, 0, 5)] = BitPawn(0, 5, kWhite),
  [BitLocate(kWhite, kPawn, 0, 6)] = BitPawn(0, 6, kWhite),
  [BitLocate(kWhite, kPawn, 0, 7)] = BitPawn(0, 7, kWhite),
  [BitLocate(kWhite, kPawn, 1, 0)] = BitPawn(1, 0, kWhite),
  [BitLocate(kWhite, kPawn, 1, 1)] = BitPawn(1, 1, kWhite),
  [BitLocate(kWhite, kPawn, 1, 2)] = BitPawn(1, 2, kWhite),
  [BitLocate(kWhite, kPawn, 1, 3)] = BitPawn(1, 3, kWhite),
  [BitLocate(kWhite, kPawn, 1, 4)] = BitPawn(1, 4, kWhite),
  [BitLocate(kWhite, kPawn, 1, 5)] = BitPawn(1, 5, kWhite),
  [BitLocate(kWhite, kPawn, 1, 6)] = BitPawn(1, 6, kWhite),
  [BitLocate(kWhite, kPawn, 1, 7)] = BitPawn(1, 7, kWhite),
  [BitLocate(kWhite, kPawn, 2, 0)] = BitPawn(2, 0, kWhite),
  [BitLocate(kWhite, kPawn, 2, 1)] = BitPawn(2, 1, kWhite),
  [BitLocate(kWhite, kPawn, 2, 2)] = BitPawn(2, 2, kWhite),
  [BitLocate(kWhite, kPawn, 2, 3)] = BitPawn(2, 3, kWhite),
  [BitLocate(kWhite, kPawn, 2, 4)] = BitPawn(2, 4, kWhite),
  [BitLocate(kWhite, kPawn, 2, 5)] = BitPawn(2, 5, kWhite),
  [BitLocate(kWhite, kPawn, 2, 6)] = BitPawn(2, 6, kWhite),
  [BitLocate(kWhite, kPawn, 2, 7)] = BitPawn(2, 7, kWhite),
  [BitLocate(kWhite, kPawn, 3, 0)] = BitPawn(3, 0, kWhite),
  [BitLocate(kWhite, kPawn, 3, 1)] = BitPawn(3, 1, kWhite),
  [BitLocate(kWhite, kPawn, 3, 2)] = BitPawn(3, 2, kWhite),
  [BitLocate(kWhite, kPawn, 3, 3)] = BitPawn(3, 3, kWhite),
  [BitLocate(kWhite, kPawn, 3, 4)] = BitPawn(3, 4, kWhite),
  [BitLocate(kWhite, kPawn, 3, 5)] = BitPawn(3, 5, kWhite),
  [BitLocate(kWhite, kPawn, 3, 6)] = BitPawn(3, 6, kWhite),
  [BitLocate(kWhite, kPawn, 3, 7)] = BitPawn(3, 7, kWhite),
  [BitLocate(kWhite, kPawn, 4, 0)] = BitPawn(4, 0, kWhite),
  [BitLocate(kWhite, kPawn, 4, 1)] = BitPawn(4, 1, kWhite),
  [BitLocate(kWhite, kPawn, 4, 2)] = BitPawn(4, 2, kWhite),
  [BitLocate(kWhite, kPawn, 4, 3)] = BitPawn(4, 3, kWhite),
  [BitLocate(kWhite, kPawn, 4, 4)] = BitPawn(4, 4, kWhite),
  [BitLocate(kWhite, kPawn, 4, 5)] = BitPawn(4, 5, kWhite),
  [BitLocate(kWhite, kPawn, 4, 6)] = BitPawn(4, 6, kWhite),
  [BitLocate(kWhite, kPawn, 4, 7)] = BitPawn(4, 7, kWhite),
  [BitLocate(kWhite, kPawn, 5, 0)] = BitPawn(5, 0, kWhite),
  [BitLocate(kWhite, kPawn, 5, 1)] = BitPawn(5, 1, kWhite),
  [BitLocate(kWhite, kPawn, 5, 2)] = BitPawn(5, 2, kWhite),
  [BitLocate(kWhite, kPawn, 5, 3)] = BitPawn(5, 3, kWhite),
  [BitLocate(kWhite, kPawn, 5, 4)] = BitPawn(5, 4, kWhite),
  [BitLocate(kWhite, kPawn, 5, 5)] = BitPawn(5, 5, kWhite),
  [BitLocate(kWhite, kPawn, 5, 6)] = BitPawn(5, 6, kWhite),
  [BitLocate(kWhite, kPawn, 5, 7)] = BitPawn(5, 7, kWhite),
  [BitLocate(kWhite, kPawn, 6, 0)] = BitPawn(6, 0, kWhite),
  [BitLocate(kWhite, kPawn, 6, 1)] = BitPawn(6, 1, kWhite),
  [BitLocate(kWhite, kPawn, 6, 2)] = BitPawn(6, 2, kWhite),
  [BitLocate(kWhite, kPawn, 6, 3)] = BitPawn(6, 3, kWhite),
  [BitLocate(kWhite, kPawn, 6, 4)] = BitPawn(6, 4, kWhite),
  [BitLocate(kWhite, kPawn, 6, 5)] = BitPawn(6, 5, kWhite),
  [BitLocate(kWhite, kPawn, 6, 6)] = BitPawn(6, 6, kWhite),
  [BitLocate(kWhite, kPawn, 6, 7)] = BitPawn(6, 7, kWhite),
  [BitLocate(kWhite, kPawn, 7, 0)] = BitPawn(7, 0, kWhite),
  [BitLocate(kWhite, kPawn, 7, 1)] = BitPawn(7, 1, kWhite),
  [BitLocate(kWhite, kPawn, 7, 2)] = BitPawn(7, 2, kWhite),
  [BitLocate(kWhite, kPawn, 7, 3)] = BitPawn(7, 3, kWhite),
  [BitLocate(kWhite, kPawn, 7, 4)] = BitPawn(7, 4, kWhite),
  [BitLocate(kWhite, kPawn, 7, 5)] = BitPawn(7, 5, kWhite),
  [BitLocate(kWhite, kPawn, 7, 6)] = BitPawn(7, 6, kWhite),
  [BitLocate(kWhite, kPawn, 7, 7)] = BitPawn(7, 7, kWhite),
  [BitLocate(kBlack, kPawn, 0, 0)] = BitPawn(0, 0, kBlack),
  [BitLocate(kBlack, kPawn, 0, 1)] = BitPawn(0, 1, kBlack),
  [BitLocate(kBlack, kPawn, 0, 2)] = BitPawn(0, 2, kBlack),
  [BitLocate(kBlack, kPawn, 0, 3)] = BitPawn(0, 3, kBlack),
  [BitLocate(kBlack, kPawn, 0, 4)] = BitPawn(0, 4, kBlack),
  [BitLocate(kBlack, kPawn, 0, 5)] = BitPawn(0, 5, kBlack),
  [BitLocate(kBlack, kPawn, 0, 6)] = BitPawn(0, 6, kBlack),
  [BitLocate(kBlack, kPawn, 0, 7)] = BitPawn(0, 7, kBlack),
  [BitLocate(kBlack, kPawn, 1, 0)] = BitPawn(1, 0, kBlack),
  [BitLocate(kBlack, kPawn, 1, 1)] = BitPawn(1, 1, kBlack),
  [BitLocate(kBlack, kPawn, 1, 2)] = BitPawn(1, 2, kBlack),
  [BitLocate(kBlack, kPawn, 1, 3)] = BitPawn(1, 3, kBlack),
  [BitLocate(kBlack, kPawn, 1, 4)] = BitPawn(1, 4, kBlack),
  [BitLocate(kBlack, kPawn, 1, 5)] = BitPawn(1, 5, kBlack),
  [BitLocate(kBlack, kPawn, 1, 6)] = BitPawn(1, 6, kBlack),
  [BitLocate(kBlack, kPawn, 1, 7)] = BitPawn(1, 7, kBlack),
  [BitLocate(kBlack, kPawn, 2, 0)] = BitPawn(2, 0, kBlack),
  [BitLocate(kBlack, kPawn, 2, 1)] = BitPawn(2, 1, kBlack),
  [BitLocate(kBlack, kPawn, 2, 2)] = BitPawn(2, 2, kBlack),
  [BitLocate(kBlack, kPawn, 2, 3)] = BitPawn(2, 3, kBlack),
  [BitLocate(kBlack, kPawn, 2, 4)] = BitPawn(2, 4, kBlack),
  [BitLocate(kBlack, kPawn, 2, 5)] = BitPawn(2, 5, kBlack),
  [BitLocate(kBlack, kPawn, 2, 6)] = BitPawn(2, 6, kBlack),
  [BitLocate(kBlack, kPawn, 2, 7)] = BitPawn(2, 7, kBlack),
  [BitLocate(kBlack, kPawn, 3, 0)] = BitPawn(3, 0, kBlack),
  [BitLocate(kBlack, kPawn, 3, 1)] = BitPawn(3, 1, kBlack),
  [BitLocate(kBlack, kPawn, 3, 2)] = BitPawn(3, 2, kBlack),
  [BitLocate(kBlack, kPawn, 3, 3)] = BitPawn(3, 3, kBlack),
  [BitLocate(kBlack, kPawn, 3, 4)] = BitPawn(3, 4, kBlack),
  [BitLocate(kBlack, kPawn, 3, 5)] = BitPawn(3, 5, kBlack),
  [BitLocate(kBlack, kPawn, 3, 6)] = BitPawn(3, 6, kBlack),
  [BitLocate(kBlack, kPawn, 3, 7)] = BitPawn(3, 7, kBlack),
  [BitLocate(kBlack, kPawn, 4, 0)] = BitPawn(4, 0, kBlack),
  [BitLocate(kBlack, kPawn, 4, 1)] = BitPawn(4, 1, kBlack),
  [BitLocate(kBlack, kPawn, 4, 2)] = BitPawn(4, 2, kBlack),
  [BitLocate(kBlack, kPawn, 4, 3)] = BitPawn(4, 3, kBlack),
  [BitLocate(kBlack, kPawn, 4, 4)] = BitPawn(4, 4, kBlack),
  [BitLocate(kBlack, kPawn, 4, 5)] = BitPawn(4, 5, kBlack),
  [BitLocate(kBlack, kPawn, 4, 6)] = BitPawn(4, 6, kBlack),
  [BitLocate(kBlack, kPawn, 4, 7)] = BitPawn(4, 7, kBlack),
  [BitLocate(kBlack, kPawn, 5, 0)] = BitPawn(5, 0, kBlack),
  [BitLocate(kBlack, kPawn, 5, 1)] = BitPawn(5, 1, kBlack),
  [BitLocate(kBlack, kPawn, 5, 2)] = BitPawn(5, 2, kBlack),
  [BitLocate(kBlack, kPawn, 5, 3)] = BitPawn(5, 3, kBlack),
  [BitLocate(kBlack, kPawn, 5, 4)] = BitPawn(5, 4, kBlack),
  [BitLocate(kBlack, kPawn, 5, 5)] = BitPawn(5, 5, kBlack),
  [BitLocate(kBlack, kPawn, 5, 6)] = BitPawn(5, 6, kBlack),
  [BitLocate(kBlack, kPawn, 5, 7)] = BitPawn(5, 7, kBlack),
  [BitLocate(kBlack, kPawn, 6, 0)] = BitPawn(6, 0, kBlack),
  [BitLocate(kBlack, kPawn, 6, 1)] = BitPawn(6, 1, kBlack),
  [BitLocate(kBlack, kPawn, 6, 2)] = BitPawn(6, 2, kBlack),
  [BitLocate(kBlack, kPawn, 6, 3)] = BitPawn(6, 3, kBlack),
  [BitLocate(kBlack, kPawn, 6, 4)] = BitPawn(6, 4, kBlack),
  [BitLocate(kBlack, kPawn, 6, 5)] = BitPawn(6, 5, kBlack),
  [BitLocate(kBlack, kPawn, 6, 6)] = BitPawn(6, 6, kBlack),
  [BitLocate(kBlack, kPawn, 6, 7)] = BitPawn(6, 7, kBlack),
  [BitLocate(kBlack, kPawn, 7, 0)] = BitPawn(7, 0, kBlack),
  [BitLocate(kBlack, kPawn, 7, 1)] = BitPawn(7, 1, kBlack),
  [BitLocate(kBlack, kPawn, 7, 2)] = BitPawn(7, 2, kBlack),
  [BitLocate(kBlack, kPawn, 7, 3)] = BitPawn(7, 3, kBlack),
  [BitLocate(kBlack, kPawn, 7, 4)] = BitPawn(7, 4, kBlack),
  [BitLocate(kBlack, kPawn, 7, 5)] = BitPawn(7, 5, kBlack),
  [BitLocate(kBlack, kPawn, 7, 6)] = BitPawn(7, 6, kBlack),
  [BitLocate(kBlack, kPawn, 7, 7)] = BitPawn(7, 7, kBlack),
  [BitLocate(kWhite, kKnight, 0, 0)] = BitKnight(0, 0),
  [BitLocate(kWhite, kKnight, 0, 1)] = BitKnight(0, 1),
  [BitLocate(kWhite, kKnight, 0, 2)] = BitKnight(0, 2),
  [BitLocate(kWhite, kKnight, 0, 3)] = BitKnight(0, 3),
  [BitLocate(kWhite, kKnight, 0, 4)] = BitKnight(0, 4),
  [BitLocate(kWhite, kKnight, 0, 5)] = BitKnight(0, 5),
  [BitLocate(kWhite, kKnight, 0, 6)] = BitKnight(0, 6),
  [BitLocate(kWhite, kKnight, 0, 7)] = BitKnight(0, 7),
  [BitLocate(kWhite, kKnight, 1, 0)] = BitKnight(1, 0),
  [BitLocate(kWhite, kKnight, 1, 1)] = BitKnight(1, 1),
  [BitLocate(kWhite, kKnight, 1, 2)] = BitKnight(1, 2),
  [BitLocate(kWhite, kKnight, 1, 3)] = BitKnight(1, 3),
  [BitLocate(kWhite, kKnight, 1, 4)] = BitKnight(1, 4),
  [BitLocate(kWhite, kKnight, 1, 5)] = BitKnight(1, 5),
  [BitLocate(kWhite, kKnight, 1, 6)] = BitKnight(1, 6),
  [BitLocate(kWhite, kKnight, 1, 7)] = BitKnight(1, 7),
  [BitLocate(kWhite, kKnight, 2, 0)] = BitKnight(2, 0),
  [BitLocate(kWhite, kKnight, 2, 1)] = BitKnight(2, 1),
  [BitLocate(kWhite, kKnight, 2, 2)] = BitKnight(2, 2),
  [BitLocate(kWhite, kKnight, 2, 3)] = BitKnight(2, 3),
  [BitLocate(kWhite, kKnight, 2, 4)] = BitKnight(2, 4),
  [BitLocate(kWhite, kKnight, 2, 5)] = BitKnight(2, 5),
  [BitLocate(kWhite, kKnight, 2, 6)] = BitKnight(2, 6),
  [BitLocate(kWhite, kKnight, 2, 7)] = BitKnight(2, 7),
  [BitLocate(kWhite, kKnight, 3, 0)] = BitKnight(3, 0),
  [BitLocate(kWhite, kKnight, 3, 1)] = BitKnight(3, 1),
  [BitLocate(kWhite, kKnight, 3, 2)] = BitKnight(3, 2),
  [BitLocate(kWhite, kKnight, 3, 3)] = BitKnight(3, 3),
  [BitLocate(kWhite, kKnight, 3, 4)] = BitKnight(3, 4),
  [BitLocate(kWhite, kKnight, 3, 5)] = BitKnight(3, 5),
  [BitLocate(kWhite, kKnight, 3, 6)] = BitKnight(3, 6),
  [BitLocate(kWhite, kKnight, 3, 7)] = BitKnight(3, 7),
  [BitLocate(kWhite, kKnight, 4, 0)] = BitKnight(4, 0),
  [BitLocate(kWhite, kKnight, 4, 1)] = BitKnight(4, 1),
  [BitLocate(kWhite, kKnight, 4, 2)] = BitKnight(4, 2),
  [BitLocate(kWhite, kKnight, 4, 3)] = BitKnight(4, 3),
  [BitLocate(kWhite, kKnight, 4, 4)] = BitKnight(4, 4),
  [BitLocate(kWhite, kKnight, 4, 5)] = BitKnight(4, 5),
  [BitLocate(kWhite, kKnight, 4, 6)] = BitKnight(4, 6),
  [BitLocate(kWhite, kKnight, 4, 7)] = BitKnight(4, 7),
  [BitLocate(kWhite, kKnight, 5, 0)] = BitKnight(5, 0),
  [BitLocate(kWhite, kKnight, 5, 1)] = BitKnight(5, 1),
  [BitLocate(kWhite, kKnight, 5, 2)] = BitKnight(5, 2),
  [BitLocate(kWhite, kKnight, 5, 3)] = BitKnight(5, 3),
  [BitLocate(kWhite, kKnight, 5, 4)] = BitKnight(5, 4),
  [BitLocate(kWhite, kKnight, 5, 5)] = BitKnight(5, 5),
  [BitLocate(kWhite, kKnight, 5, 6)] = BitKnight(5, 6),
  [BitLocate(kWhite, kKnight, 5, 7)] = BitKnight(5, 7),
  [BitLocate(kWhite, kKnight, 6, 0)] = BitKnight(6, 0),
  [BitLocate(kWhite, kKnight, 6, 1)] = BitKnight(6, 1),
  [BitLocate(kWhite, kKnight, 6, 2)] = BitKnight(6, 2),
  [BitLocate(kWhite, kKnight, 6, 3)] = BitKnight(6, 3),
  [BitLocate(kWhite, kKnight, 6, 4)] = BitKnight(6, 4),
  [BitLocate(kWhite, kKnight, 6, 5)] = BitKnight(6, 5),
  [BitLocate(kWhite, kKnight, 6, 6)] = BitKnight(6, 6),
  [BitLocate(kWhite, kKnight, 6, 7)] = BitKnight(6, 7),
  [BitLocate(kWhite, kKnight, 7, 0)] = BitKnight(7, 0),
  [BitLocate(kWhite, kKnight, 7, 1)] = BitKnight(7, 1),
  [BitLocate(kWhite, kKnight, 7, 2)] = BitKnight(7, 2),
  [BitLocate(kWhite, kKnight, 7, 3)] = BitKnight(7, 3),
  [BitLocate(kWhite, kKnight, 7, 4)] = BitKnight(7, 4),
  [BitLocate(kWhite, kKnight, 7, 5)] = BitKnight(7, 5),
  [BitLocate(kWhite, kKnight, 7, 6)] = BitKnight(7, 6),
  [BitLocate(kWhite, kKnight, 7, 7)] = BitKnight(7, 7),
  [BitLocate(kBlack, kKnight, 0, 0)] = BitKnight(0, 0),
  [BitLocate(kBlack, kKnight, 0, 1)] = BitKnight(0, 1),
  [BitLocate(kBlack, kKnight, 0, 2)] = BitKnight(0, 2),
  [BitLocate(kBlack, kKnight, 0, 3)] = BitKnight(0, 3),
  [BitLocate(kBlack, kKnight, 0, 4)] = BitKnight(0, 4),
  [BitLocate(kBlack, kKnight, 0, 5)] = BitKnight(0, 5),
  [BitLocate(kBlack, kKnight, 0, 6)] = BitKnight(0, 6),
  [BitLocate(kBlack, kKnight, 0, 7)] = BitKnight(0, 7),
  [BitLocate(kBlack, kKnight, 1, 0)] = BitKnight(1, 0),
  [BitLocate(kBlack, kKnight, 1, 1)] = BitKnight(1, 1),
  [BitLocate(kBlack, kKnight, 1, 2)] = BitKnight(1, 2),
  [BitLocate(kBlack, kKnight, 1, 3)] = BitKnight(1, 3),
  [BitLocate(kBlack, kKnight, 1, 4)] = BitKnight(1, 4),
  [BitLocate(kBlack, kKnight, 1, 5)] = BitKnight(1, 5),
  [BitLocate(kBlack, kKnight, 1, 6)] = BitKnight(1, 6),
  [BitLocate(kBlack, kKnight, 1, 7)] = BitKnight(1, 7),
  [BitLocate(kBlack, kKnight, 2, 0)] = BitKnight(2, 0),
  [BitLocate(kBlack, kKnight, 2, 1)] = BitKnight(2, 1),
  [BitLocate(kBlack, kKnight, 2, 2)] = BitKnight(2, 2),
  [BitLocate(kBlack, kKnight, 2, 3)] = BitKnight(2, 3),
  [BitLocate(kBlack, kKnight, 2, 4)] = BitKnight(2, 4),
  [BitLocate(kBlack, kKnight, 2, 5)] = BitKnight(2, 5),
  [BitLocate(kBlack, kKnight, 2, 6)] = BitKnight(2, 6),
  [BitLocate(kBlack, kKnight, 2, 7)] = BitKnight(2, 7),
  [BitLocate(kBlack, kKnight, 3, 0)] = BitKnight(3, 0),
  [BitLocate(kBlack, kKnight, 3, 1)] = BitKnight(3, 1),
  [BitLocate(kBlack, kKnight, 3, 2)] = BitKnight(3, 2),
  [BitLocate(kBlack, kKnight, 3, 3)] = BitKnight(3, 3),
  [BitLocate(kBlack, kKnight, 3, 4)] = BitKnight(3, 4),
  [BitLocate(kBlack, kKnight, 3, 5)] = BitKnight(3, 5),
  [BitLocate(kBlack, kKnight, 3, 6)] = BitKnight(3, 6),
  [BitLocate(kBlack, kKnight, 3, 7)] = BitKnight(3, 7),
  [BitLocate(kBlack, kKnight, 4, 0)] = BitKnight(4, 0),
  [BitLocate(kBlack, kKnight, 4, 1)] = BitKnight(4, 1),
  [BitLocate(kBlack, kKnight, 4, 2)] = BitKnight(4, 2),
  [BitLocate(kBlack, kKnight, 4, 3)] = BitKnight(4, 3),
  [BitLocate(kBlack, kKnight, 4, 4)] = BitKnight(4, 4),
  [BitLocate(kBlack, kKnight, 4, 5)] = BitKnight(4, 5),
  [BitLocate(kBlack, kKnight, 4, 6)] = BitKnight(4, 6),
  [BitLocate(kBlack, kKnight, 4, 7)] = BitKnight(4, 7),
  [BitLocate(kBlack, kKnight, 5, 0)] = BitKnight(5, 0),
  [BitLocate(kBlack, kKnight, 5, 1)] = BitKnight(5, 1),
  [BitLocate(kBlack, kKnight, 5, 2)] = BitKnight(5, 2),
  [BitLocate(kBlack, kKnight, 5, 3)] = BitKnight(5, 3),
  [BitLocate(kBlack, kKnight, 5, 4)] = BitKnight(5, 4),
  [BitLocate(kBlack, kKnight, 5, 5)] = BitKnight(5, 5),
  [BitLocate(kBlack, kKnight, 5, 6)] = BitKnight(5, 6),
  [BitLocate(kBlack, kKnight, 5, 7)] = BitKnight(5, 7),
  [BitLocate(kBlack, kKnight, 6, 0)] = BitKnight(6, 0),
  [BitLocate(kBlack, kKnight, 6, 1)] = BitKnight(6, 1),
  [BitLocate(kBlack, kKnight, 6, 2)] = BitKnight(6, 2),
  [BitLocate(kBlack, kKnight, 6, 3)] = BitKnight(6, 3),
  [BitLocate(kBlack, kKnight, 6, 4)] = BitKnight(6, 4),
  [BitLocate(kBlack, kKnight, 6, 5)] = BitKnight(6, 5),
  [BitLocate(kBlack, kKnight, 6, 6)] = BitKnight(6, 6),
  [BitLocate(kBlack, kKnight, 6, 7)] = BitKnight(6, 7),
  [BitLocate(kBlack, kKnight, 7, 0)] = BitKnight(7, 0),
  [BitLocate(kBlack, kKnight, 7, 1)] = BitKnight(7, 1),
  [BitLocate(kBlack, kKnight, 7, 2)] = BitKnight(7, 2),
  [BitLocate(kBlack, kKnight, 7, 3)] = BitKnight(7, 3),
  [BitLocate(kBlack, kKnight, 7, 4)] = BitKnight(7, 4),
  [BitLocate(kBlack, kKnight, 7, 5)] = BitKnight(7, 5),
  [BitLocate(kBlack, kKnight, 7, 6)] = BitKnight(7, 6),
  [BitLocate(kBlack, kKnight, 7, 7)] = BitKnight(7, 7),
  [BitLocate(kWhite, kBishop, 0, 0)] = BitBishop(0, 0),
  [BitLocate(kWhite, kBishop, 0, 1)] = BitBishop(0, 1),
  [BitLocate(kWhite, kBishop, 0, 2)] = BitBishop(0, 2),
  [BitLocate(kWhite, kBishop, 0, 3)] = BitBishop(0, 3),
  [BitLocate(kWhite, kBishop, 0, 4)] = BitBishop(0, 4),
  [BitLocate(kWhite, kBishop, 0, 5)] = BitBishop(0, 5),
  [BitLocate(kWhite, kBishop, 0, 6)] = BitBishop(0, 6),
  [BitLocate(kWhite, kBishop, 0, 7)] = BitBishop(0, 7),
  [BitLocate(kWhite, kBishop, 1, 0)] = BitBishop(1, 0),
  [BitLocate(kWhite, kBishop, 1, 1)] = BitBishop(1, 1),
  [BitLocate(kWhite, kBishop, 1, 2)] = BitBishop(1, 2),
  [BitLocate(kWhite, kBishop, 1, 3)] = BitBishop(1, 3),
  [BitLocate(kWhite, kBishop, 1, 4)] = BitBishop(1, 4),
  [BitLocate(kWhite, kBishop, 1, 5)] = BitBishop(1, 5),
  [BitLocate(kWhite, kBishop, 1, 6)] = BitBishop(1, 6),
  [BitLocate(kWhite, kBishop, 1, 7)] = BitBishop(1, 7),
  [BitLocate(kWhite, kBishop, 2, 0)] = BitBishop(2, 0),
  [BitLocate(kWhite, kBishop, 2, 1)] = BitBishop(2, 1),
  [BitLocate(kWhite, kBishop, 2, 2)] = BitBishop(2, 2),
  [BitLocate(kWhite, kBishop, 2, 3)] = BitBishop(2, 3),
  [BitLocate(kWhite, kBishop, 2, 4)] = BitBishop(2, 4),
  [BitLocate(kWhite, kBishop, 2, 5)] = BitBishop(2, 5),
  [BitLocate(kWhite, kBishop, 2, 6)] = BitBishop(2, 6),
  [BitLocate(kWhite, kBishop, 2, 7)] = BitBishop(2, 7),
  [BitLocate(kWhite, kBishop, 3, 0)] = BitBishop(3, 0),
  [BitLocate(kWhite, kBishop, 3, 1)] = BitBishop(3, 1),
  [BitLocate(kWhite, kBishop, 3, 2)] = BitBishop(3, 2),
  [BitLocate(kWhite, kBishop, 3, 3)] = BitBishop(3, 3),
  [BitLocate(kWhite, kBishop, 3, 4)] = BitBishop(3, 4),
  [BitLocate(kWhite, kBishop, 3, 5)] = BitBishop(3, 5),
  [BitLocate(kWhite, kBishop, 3, 6)] = BitBishop(3, 6),
  [BitLocate(kWhite, kBishop, 3, 7)] = BitBishop(3, 7),
  [BitLocate(kWhite, kBishop, 4, 0)] = BitBishop(4, 0),
  [BitLocate(kWhite, kBishop, 4, 1)] = BitBishop(4, 1),
  [BitLocate(kWhite, kBishop, 4, 2)] = BitBishop(4, 2),
  [BitLocate(kWhite, kBishop, 4, 3)] = BitBishop(4, 3),
  [BitLocate(kWhite, kBishop, 4, 4)] = BitBishop(4, 4),
  [BitLocate(kWhite, kBishop, 4, 5)] = BitBishop(4, 5),
  [BitLocate(kWhite, kBishop, 4, 6)] = BitBishop(4, 6),
  [BitLocate(kWhite, kBishop, 4, 7)] = BitBishop(4, 7),
  [BitLocate(kWhite, kBishop, 5, 0)] = BitBishop(5, 0),
  [BitLocate(kWhite, kBishop, 5, 1)] = BitBishop(5, 1),
  [BitLocate(kWhite, kBishop, 5, 2)] = BitBishop(5, 2),
  [BitLocate(kWhite, kBishop, 5, 3)] = BitBishop(5, 3),
  [BitLocate(kWhite, kBishop, 5, 4)] = BitBishop(5, 4),
  [BitLocate(kWhite, kBishop, 5, 5)] = BitBishop(5, 5),
  [BitLocate(kWhite, kBishop, 5, 6)] = BitBishop(5, 6),
  [BitLocate(kWhite, kBishop, 5, 7)] = BitBishop(5, 7),
  [BitLocate(kWhite, kBishop, 6, 0)] = BitBishop(6, 0),
  [BitLocate(kWhite, kBishop, 6, 1)] = BitBishop(6, 1),
  [BitLocate(kWhite, kBishop, 6, 2)] = BitBishop(6, 2),
  [BitLocate(kWhite, kBishop, 6, 3)] = BitBishop(6, 3),
  [BitLocate(kWhite, kBishop, 6, 4)] = BitBishop(6, 4),
  [BitLocate(kWhite, kBishop, 6, 5)] = BitBishop(6, 5),
  [BitLocate(kWhite, kBishop, 6, 6)] = BitBishop(6, 6),
  [BitLocate(kWhite, kBishop, 6, 7)] = BitBishop(6, 7),
  [BitLocate(kWhite, kBishop, 7, 0)] = BitBishop(7, 0),
  [BitLocate(kWhite, kBishop, 7, 1)] = BitBishop(7, 1),
  [BitLocate(kWhite, kBishop, 7, 2)] = BitBishop(7, 2),
  [BitLocate(kWhite, kBishop, 7, 3)] = BitBishop(7, 3),
  [BitLocate(kWhite, kBishop, 7, 4)] = BitBishop(7, 4),
  [BitLocate(kWhite, kBishop, 7, 5)] = BitBishop(7, 5),
  [BitLocate(kWhite, kBishop, 7, 6)] = BitBishop(7, 6),
  [BitLocate(kWhite, kBishop, 7, 7)] = BitBishop(7, 7),
  [BitLocate(kBlack, kBishop, 0, 0)] = BitBishop(0, 0),
  [BitLocate(kBlack, kBishop, 0, 1)] = BitBishop(0, 1),
  [BitLocate(kBlack, kBishop, 0, 2)] = BitBishop(0, 2),
  [BitLocate(kBlack, kBishop, 0, 3)] = BitBishop(0, 3),
  [BitLocate(kBlack, kBishop, 0, 4)] = BitBishop(0, 4),
  [BitLocate(kBlack, kBishop, 0, 5)] = BitBishop(0, 5),
  [BitLocate(kBlack, kBishop, 0, 6)] = BitBishop(0, 6),
  [BitLocate(kBlack, kBishop, 0, 7)] = BitBishop(0, 7),
  [BitLocate(kBlack, kBishop, 1, 0)] = BitBishop(1, 0),
  [BitLocate(kBlack, kBishop, 1, 1)] = BitBishop(1, 1),
  [BitLocate(kBlack, kBishop, 1, 2)] = BitBishop(1, 2),
  [BitLocate(kBlack, kBishop, 1, 3)] = BitBishop(1, 3),
  [BitLocate(kBlack, kBishop, 1, 4)] = BitBishop(1, 4),
  [BitLocate(kBlack, kBishop, 1, 5)] = BitBishop(1, 5),
  [BitLocate(kBlack, kBishop, 1, 6)] = BitBishop(1, 6),
  [BitLocate(kBlack, kBishop, 1, 7)] = BitBishop(1, 7),
  [BitLocate(kBlack, kBishop, 2, 0)] = BitBishop(2, 0),
  [BitLocate(kBlack, kBishop, 2, 1)] = BitBishop(2, 1),
  [BitLocate(kBlack, kBishop, 2, 2)] = BitBishop(2, 2),
  [BitLocate(kBlack, kBishop, 2, 3)] = BitBishop(2, 3),
  [BitLocate(kBlack, kBishop, 2, 4)] = BitBishop(2, 4),
  [BitLocate(kBlack, kBishop, 2, 5)] = BitBishop(2, 5),
  [BitLocate(kBlack, kBishop, 2, 6)] = BitBishop(2, 6),
  [BitLocate(kBlack, kBishop, 2, 7)] = BitBishop(2, 7),
  [BitLocate(kBlack, kBishop, 3, 0)] = BitBishop(3, 0),
  [BitLocate(kBlack, kBishop, 3, 1)] = BitBishop(3, 1),
  [BitLocate(kBlack, kBishop, 3, 2)] = BitBishop(3, 2),
  [BitLocate(kBlack, kBishop, 3, 3)] = BitBishop(3, 3),
  [BitLocate(kBlack, kBishop, 3, 4)] = BitBishop(3, 4),
  [BitLocate(kBlack, kBishop, 3, 5)] = BitBishop(3, 5),
  [BitLocate(kBlack, kBishop, 3, 6)] = BitBishop(3, 6),
  [BitLocate(kBlack, kBishop, 3, 7)] = BitBishop(3, 7),
  [BitLocate(kBlack, kBishop, 4, 0)] = BitBishop(4, 0),
  [BitLocate(kBlack, kBishop, 4, 1)] = BitBishop(4, 1),
  [BitLocate(kBlack, kBishop, 4, 2)] = BitBishop(4, 2),
  [BitLocate(kBlack, kBishop, 4, 3)] = BitBishop(4, 3),
  [BitLocate(kBlack, kBishop, 4, 4)] = BitBishop(4, 4),
  [BitLocate(kBlack, kBishop, 4, 5)] = BitBishop(4, 5),
  [BitLocate(kBlack, kBishop, 4, 6)] = BitBishop(4, 6),
  [BitLocate(kBlack, kBishop, 4, 7)] = BitBishop(4, 7),
  [BitLocate(kBlack, kBishop, 5, 0)] = BitBishop(5, 0),
  [BitLocate(kBlack, kBishop, 5, 1)] = BitBishop(5, 1),
  [BitLocate(kBlack, kBishop, 5, 2)] = BitBishop(5, 2),
  [BitLocate(kBlack, kBishop, 5, 3)] = BitBishop(5, 3),
  [BitLocate(kBlack, kBishop, 5, 4)] = BitBishop(5, 4),
  [BitLocate(kBlack, kBishop, 5, 5)] = BitBishop(5, 5),
  [BitLocate(kBlack, kBishop, 5, 6)] = BitBishop(5, 6),
  [BitLocate(kBlack, kBishop, 5, 7)] = BitBishop(5, 7),
  [BitLocate(kBlack, kBishop, 6, 0)] = BitBishop(6, 0),
  [BitLocate(kBlack, kBishop, 6, 1)] = BitBishop(6, 1),
  [BitLocate(kBlack, kBishop, 6, 2)] = BitBishop(6, 2),
  [BitLocate(kBlack, kBishop, 6, 3)] = BitBishop(6, 3),
  [BitLocate(kBlack, kBishop, 6, 4)] = BitBishop(6, 4),
  [BitLocate(kBlack, kBishop, 6, 5)] = BitBishop(6, 5),
  [BitLocate(kBlack, kBishop, 6, 6)] = BitBishop(6, 6),
  [BitLocate(kBlack, kBishop, 6, 7)] = BitBishop(6, 7),
  [BitLocate(kBlack, kBishop, 7, 0)] = BitBishop(7, 0),
  [BitLocate(kBlack, kBishop, 7, 1)] = BitBishop(7, 1),
  [BitLocate(kBlack, kBishop, 7, 2)] = BitBishop(7, 2),
  [BitLocate(kBlack, kBishop, 7, 3)] = BitBishop(7, 3),
  [BitLocate(kBlack, kBishop, 7, 4)] = BitBishop(7, 4),
  [BitLocate(kBlack, kBishop, 7, 5)] = BitBishop(7, 5),
  [BitLocate(kBlack, kBishop, 7, 6)] = BitBishop(7, 6),
  [BitLocate(kBlack, kBishop, 7, 7)] = BitBishop(7, 7),
  [BitLocate(kWhite, kRook, 0, 0)] = BitRook(0, 0),
  [BitLocate(kWhite, kRook, 0, 1)] = BitRook(0, 1),
  [BitLocate(kWhite, kRook, 0, 2)] = BitRook(0, 2),
  [BitLocate(kWhite, kRook, 0, 3)] = BitRook(0, 3),
  [BitLocate(kWhite, kRook, 0, 4)] = BitRook(0, 4),
  [BitLocate(kWhite, kRook, 0, 5)] = BitRook(0, 5),
  [BitLocate(kWhite, kRook, 0, 6)] = BitRook(0, 6),
  [BitLocate(kWhite, kRook, 0, 7)] = BitRook(0, 7),
  [BitLocate(kWhite, kRook, 1, 0)] = BitRook(1, 0),
  [BitLocate(kWhite, kRook, 1, 1)] = BitRook(1, 1),
  [BitLocate(kWhite, kRook, 1, 2)] = BitRook(1, 2),
  [BitLocate(kWhite, kRook, 1, 3)] = BitRook(1, 3),
  [BitLocate(kWhite, kRook, 1, 4)] = BitRook(1, 4),
  [BitLocate(kWhite, kRook, 1, 5)] = BitRook(1, 5),
  [BitLocate(kWhite, kRook, 1, 6)] = BitRook(1, 6),
  [BitLocate(kWhite, kRook, 1, 7)] = BitRook(1, 7),
  [BitLocate(kWhite, kRook, 2, 0)] = BitRook(2, 0),
  [BitLocate(kWhite, kRook, 2, 1)] = BitRook(2, 1),
  [BitLocate(kWhite, kRook, 2, 2)] = BitRook(2, 2),
  [BitLocate(kWhite, kRook, 2, 3)] = BitRook(2, 3),
  [BitLocate(kWhite, kRook, 2, 4)] = BitRook(2, 4),
  [BitLocate(kWhite, kRook, 2, 5)] = BitRook(2, 5),
  [BitLocate(kWhite, kRook, 2, 6)] = BitRook(2, 6),
  [BitLocate(kWhite, kRook, 2, 7)] = BitRook(2, 7),
  [BitLocate(kWhite, kRook, 3, 0)] = BitRook(3, 0),
  [BitLocate(kWhite, kRook, 3, 1)] = BitRook(3, 1),
  [BitLocate(kWhite, kRook, 3, 2)] = BitRook(3, 2),
  [BitLocate(kWhite, kRook, 3, 3)] = BitRook(3, 3),
  [BitLocate(kWhite, kRook, 3, 4)] = BitRook(3, 4),
  [BitLocate(kWhite, kRook, 3, 5)] = BitRook(3, 5),
  [BitLocate(kWhite, kRook, 3, 6)] = BitRook(3, 6),
  [BitLocate(kWhite, kRook, 3, 7)] = BitRook(3, 7),
  [BitLocate(kWhite, kRook, 4, 0)] = BitRook(4, 0),
  [BitLocate(kWhite, kRook, 4, 1)] = BitRook(4, 1),
  [BitLocate(kWhite, kRook, 4, 2)] = BitRook(4, 2),
  [BitLocate(kWhite, kRook, 4, 3)] = BitRook(4, 3),
  [BitLocate(kWhite, kRook, 4, 4)] = BitRook(4, 4),
  [BitLocate(kWhite, kRook, 4, 5)] = BitRook(4, 5),
  [BitLocate(kWhite, kRook, 4, 6)] = BitRook(4, 6),
  [BitLocate(kWhite, kRook, 4, 7)] = BitRook(4, 7),
  [BitLocate(kWhite, kRook, 5, 0)] = BitRook(5, 0),
  [BitLocate(kWhite, kRook, 5, 1)] = BitRook(5, 1),
  [BitLocate(kWhite, kRook, 5, 2)] = BitRook(5, 2),
  [BitLocate(kWhite, kRook, 5, 3)] = BitRook(5, 3),
  [BitLocate(kWhite, kRook, 5, 4)] = BitRook(5, 4),
  [BitLocate(kWhite, kRook, 5, 5)] = BitRook(5, 5),
  [BitLocate(kWhite, kRook, 5, 6)] = BitRook(5, 6),
  [BitLocate(kWhite, kRook, 5, 7)] = BitRook(5, 7),
  [BitLocate(kWhite, kRook, 6, 0)] = BitRook(6, 0),
  [BitLocate(kWhite, kRook, 6, 1)] = BitRook(6, 1),
  [BitLocate(kWhite, kRook, 6, 2)] = BitRook(6, 2),
  [BitLocate(kWhite, kRook, 6, 3)] = BitRook(6, 3),
  [BitLocate(kWhite, kRook, 6, 4)] = BitRook(6, 4),
  [BitLocate(kWhite, kRook, 6, 5)] = BitRook(6, 5),
  [BitLocate(kWhite, kRook, 6, 6)] = BitRook(6, 6),
  [BitLocate(kWhite, kRook, 6, 7)] = BitRook(6, 7),
  [BitLocate(kWhite, kRook, 7, 0)] = BitRook(7, 0),
  [BitLocate(kWhite, kRook, 7, 1)] = BitRook(7, 1),
  [BitLocate(kWhite, kRook, 7, 2)] = BitRook(7, 2),
  [BitLocate(kWhite, kRook, 7, 3)] = BitRook(7, 3),
  [BitLocate(kWhite, kRook, 7, 4)] = BitRook(7, 4),
  [BitLocate(kWhite, kRook, 7, 5)] = BitRook(7, 5),
  [BitLocate(kWhite, kRook, 7, 6)] = BitRook(7, 6),
  [BitLocate(kWhite, kRook, 7, 7)] = BitRook(7, 7),
  [BitLocate(kBlack, kRook, 0, 0)] = BitRook(0, 0),
  [BitLocate(kBlack, kRook, 0, 1)] = BitRook(0, 1),
  [BitLocate(kBlack, kRook, 0, 2)] = BitRook(0, 2),
  [BitLocate(kBlack, kRook, 0, 3)] = BitRook(0, 3),
  [BitLocate(kBlack, kRook, 0, 4)] = BitRook(0, 4),
  [BitLocate(kBlack, kRook, 0, 5)] = BitRook(0, 5),
  [BitLocate(kBlack, kRook, 0, 6)] = BitRook(0, 6),
  [BitLocate(kBlack, kRook, 0, 7)] = BitRook(0, 7),
  [BitLocate(kBlack, kRook, 1, 0)] = BitRook(1, 0),
  [BitLocate(kBlack, kRook, 1, 1)] = BitRook(1, 1),
  [BitLocate(kBlack, kRook, 1, 2)] = BitRook(1, 2),
  [BitLocate(kBlack, kRook, 1, 3)] = BitRook(1, 3),
  [BitLocate(kBlack, kRook, 1, 4)] = BitRook(1, 4),
  [BitLocate(kBlack, kRook, 1, 5)] = BitRook(1, 5),
  [BitLocate(kBlack, kRook, 1, 6)] = BitRook(1, 6),
  [BitLocate(kBlack, kRook, 1, 7)] = BitRook(1, 7),
  [BitLocate(kBlack, kRook, 2, 0)] = BitRook(2, 0),
  [BitLocate(kBlack, kRook, 2, 1)] = BitRook(2, 1),
  [BitLocate(kBlack, kRook, 2, 2)] = BitRook(2, 2),
  [BitLocate(kBlack, kRook, 2, 3)] = BitRook(2, 3),
  [BitLocate(kBlack, kRook, 2, 4)] = BitRook(2, 4),
  [BitLocate(kBlack, kRook, 2, 5)] = BitRook(2, 5),
  [BitLocate(kBlack, kRook, 2, 6)] = BitRook(2, 6),
  [BitLocate(kBlack, kRook, 2, 7)] = BitRook(2, 7),
  [BitLocate(kBlack, kRook, 3, 0)] = BitRook(3, 0),
  [BitLocate(kBlack, kRook, 3, 1)] = BitRook(3, 1),
  [BitLocate(kBlack, kRook, 3, 2)] = BitRook(3, 2),
  [BitLocate(kBlack, kRook, 3, 3)] = BitRook(3, 3),
  [BitLocate(kBlack, kRook, 3, 4)] = BitRook(3, 4),
  [BitLocate(kBlack, kRook, 3, 5)] = BitRook(3, 5),
  [BitLocate(kBlack, kRook, 3, 6)] = BitRook(3, 6),
  [BitLocate(kBlack, kRook, 3, 7)] = BitRook(3, 7),
  [BitLocate(kBlack, kRook, 4, 0)] = BitRook(4, 0),
  [BitLocate(kBlack, kRook, 4, 1)] = BitRook(4, 1),
  [BitLocate(kBlack, kRook, 4, 2)] = BitRook(4, 2),
  [BitLocate(kBlack, kRook, 4, 3)] = BitRook(4, 3),
  [BitLocate(kBlack, kRook, 4, 4)] = BitRook(4, 4),
  [BitLocate(kBlack, kRook, 4, 5)] = BitRook(4, 5),
  [BitLocate(kBlack, kRook, 4, 6)] = BitRook(4, 6),
  [BitLocate(kBlack, kRook, 4, 7)] = BitRook(4, 7),
  [BitLocate(kBlack, kRook, 5, 0)] = BitRook(5, 0),
  [BitLocate(kBlack, kRook, 5, 1)] = BitRook(5, 1),
  [BitLocate(kBlack, kRook, 5, 2)] = BitRook(5, 2),
  [BitLocate(kBlack, kRook, 5, 3)] = BitRook(5, 3),
  [BitLocate(kBlack, kRook, 5, 4)] = BitRook(5, 4),
  [BitLocate(kBlack, kRook, 5, 5)] = BitRook(5, 5),
  [BitLocate(kBlack, kRook, 5, 6)] = BitRook(5, 6),
  [BitLocate(kBlack, kRook, 5, 7)] = BitRook(5, 7),
  [BitLocate(kBlack, kRook, 6, 0)] = BitRook(6, 0),
  [BitLocate(kBlack, kRook, 6, 1)] = BitRook(6, 1),
  [BitLocate(kBlack, kRook, 6, 2)] = BitRook(6, 2),
  [BitLocate(kBlack, kRook, 6, 3)] = BitRook(6, 3),
  [BitLocate(kBlack, kRook, 6, 4)] = BitRook(6, 4),
  [BitLocate(kBlack, kRook, 6, 5)] = BitRook(6, 5),
  [BitLocate(kBlack, kRook, 6, 6)] = BitRook(6, 6),
  [BitLocate(kBlack, kRook, 6, 7)] = BitRook(6, 7),
  [BitLocate(kBlack, kRook, 7, 0)] = BitRook(7, 0),
  [BitLocate(kBlack, kRook, 7, 1)] = BitRook(7, 1),
  [BitLocate(kBlack, kRook, 7, 2)] = BitRook(7, 2),
  [BitLocate(kBlack, kRook, 7, 3)] = BitRook(7, 3),
  [BitLocate(kBlack, kRook, 7, 4)] = BitRook(7, 4),
  [BitLocate(kBlack, kRook, 7, 5)] = BitRook(7, 5),
  [BitLocate(kBlack, kRook, 7, 6)] = BitRook(7, 6),
  [BitLocate(kBlack, kRook, 7, 7)] = BitRook(7, 7),
  [BitLocate(kWhite, kQueen, 0, 0)] = BitQueen(0, 0),
  [BitLocate(kWhite, kQueen, 0, 1)] = BitQueen(0, 1),
  [BitLocate(kWhite, kQueen, 0, 2)] = BitQueen(0, 2),
  [BitLocate(kWhite, kQueen, 0, 3)] = BitQueen(0, 3),
  [BitLocate(kWhite, kQueen, 0, 4)] = BitQueen(0, 4),
  [BitLocate(kWhite, kQueen, 0, 5)] = BitQueen(0, 5),
  [BitLocate(kWhite, kQueen, 0, 6)] = BitQueen(0, 6),
  [BitLocate(kWhite, kQueen, 0, 7)] = BitQueen(0, 7),
  [BitLocate(kWhite, kQueen, 1, 0)] = BitQueen(1, 0),
  [BitLocate(kWhite, kQueen, 1, 1)] = BitQueen(1, 1),
  [BitLocate(kWhite, kQueen, 1, 2)] = BitQueen(1, 2),
  [BitLocate(kWhite, kQueen, 1, 3)] = BitQueen(1, 3),
  [BitLocate(kWhite, kQueen, 1, 4)] = BitQueen(1, 4),
  [BitLocate(kWhite, kQueen, 1, 5)] = BitQueen(1, 5),
  [BitLocate(kWhite, kQueen, 1, 6)] = BitQueen(1, 6),
  [BitLocate(kWhite, kQueen, 1, 7)] = BitQueen(1, 7),
  [BitLocate(kWhite, kQueen, 2, 0)] = BitQueen(2, 0),
  [BitLocate(kWhite, kQueen, 2, 1)] = BitQueen(2, 1),
  [BitLocate(kWhite, kQueen, 2, 2)] = BitQueen(2, 2),
  [BitLocate(kWhite, kQueen, 2, 3)] = BitQueen(2, 3),
  [BitLocate(kWhite, kQueen, 2, 4)] = BitQueen(2, 4),
  [BitLocate(kWhite, kQueen, 2, 5)] = BitQueen(2, 5),
  [BitLocate(kWhite, kQueen, 2, 6)] = BitQueen(2, 6),
  [BitLocate(kWhite, kQueen, 2, 7)] = BitQueen(2, 7),
  [BitLocate(kWhite, kQueen, 3, 0)] = BitQueen(3, 0),
  [BitLocate(kWhite, kQueen, 3, 1)] = BitQueen(3, 1),
  [BitLocate(kWhite, kQueen, 3, 2)] = BitQueen(3, 2),
  [BitLocate(kWhite, kQueen, 3, 3)] = BitQueen(3, 3),
  [BitLocate(kWhite, kQueen, 3, 4)] = BitQueen(3, 4),
  [BitLocate(kWhite, kQueen, 3, 5)] = BitQueen(3, 5),
  [BitLocate(kWhite, kQueen, 3, 6)] = BitQueen(3, 6),
  [BitLocate(kWhite, kQueen, 3, 7)] = BitQueen(3, 7),
  [BitLocate(kWhite, kQueen, 4, 0)] = BitQueen(4, 0),
  [BitLocate(kWhite, kQueen, 4, 1)] = BitQueen(4, 1),
  [BitLocate(kWhite, kQueen, 4, 2)] = BitQueen(4, 2),
  [BitLocate(kWhite, kQueen, 4, 3)] = BitQueen(4, 3),
  [BitLocate(kWhite, kQueen, 4, 4)] = BitQueen(4, 4),
  [BitLocate(kWhite, kQueen, 4, 5)] = BitQueen(4, 5),
  [BitLocate(kWhite, kQueen, 4, 6)] = BitQueen(4, 6),
  [BitLocate(kWhite, kQueen, 4, 7)] = BitQueen(4, 7),
  [BitLocate(kWhite, kQueen, 5, 0)] = BitQueen(5, 0),
  [BitLocate(kWhite, kQueen, 5, 1)] = BitQueen(5, 1),
  [BitLocate(kWhite, kQueen, 5, 2)] = BitQueen(5, 2),
  [BitLocate(kWhite, kQueen, 5, 3)] = BitQueen(5, 3),
  [BitLocate(kWhite, kQueen, 5, 4)] = BitQueen(5, 4),
  [BitLocate(kWhite, kQueen, 5, 5)] = BitQueen(5, 5),
  [BitLocate(kWhite, kQueen, 5, 6)] = BitQueen(5, 6),
  [BitLocate(kWhite, kQueen, 5, 7)] = BitQueen(5, 7),
  [BitLocate(kWhite, kQueen, 6, 0)] = BitQueen(6, 0),
  [BitLocate(kWhite, kQueen, 6, 1)] = BitQueen(6, 1),
  [BitLocate(kWhite, kQueen, 6, 2)] = BitQueen(6, 2),
  [BitLocate(kWhite, kQueen, 6, 3)] = BitQueen(6, 3),
  [BitLocate(kWhite, kQueen, 6, 4)] = BitQueen(6, 4),
  [BitLocate(kWhite, kQueen, 6, 5)] = BitQueen(6, 5),
  [BitLocate(kWhite, kQueen, 6, 6)] = BitQueen(6, 6),
  [BitLocate(kWhite, kQueen, 6, 7)] = BitQueen(6, 7),
  [BitLocate(kWhite, kQueen, 7, 0)] = BitQueen(7, 0),
  [BitLocate(kWhite, kQueen, 7, 1)] = BitQueen(7, 1),
  [BitLocate(kWhite, kQueen, 7, 2)] = BitQueen(7, 2),
  [BitLocate(kWhite, kQueen, 7, 3)] = BitQueen(7, 3),
  [BitLocate(kWhite, kQueen, 7, 4)] = BitQueen(7, 4),
  [BitLocate(kWhite, kQueen, 7, 5)] = BitQueen(7, 5),
  [BitLocate(kWhite, kQueen, 7, 6)] = BitQueen(7, 6),
  [BitLocate(kWhite, kQueen, 7, 7)] = BitQueen(7, 7),
  [BitLocate(kBlack, kQueen, 0, 0)] = BitQueen(0, 0),
  [BitLocate(kBlack, kQueen, 0, 1)] = BitQueen(0, 1),
  [BitLocate(kBlack, kQueen, 0, 2)] = BitQueen(0, 2),
  [BitLocate(kBlack, kQueen, 0, 3)] = BitQueen(0, 3),
  [BitLocate(kBlack, kQueen, 0, 4)] = BitQueen(0, 4),
  [BitLocate(kBlack, kQueen, 0, 5)] = BitQueen(0, 5),
  [BitLocate(kBlack, kQueen, 0, 6)] = BitQueen(0, 6),
  [BitLocate(kBlack, kQueen, 0, 7)] = BitQueen(0, 7),
  [BitLocate(kBlack, kQueen, 1, 0)] = BitQueen(1, 0),
  [BitLocate(kBlack, kQueen, 1, 1)] = BitQueen(1, 1),
  [BitLocate(kBlack, kQueen, 1, 2)] = BitQueen(1, 2),
  [BitLocate(kBlack, kQueen, 1, 3)] = BitQueen(1, 3),
  [BitLocate(kBlack, kQueen, 1, 4)] = BitQueen(1, 4),
  [BitLocate(kBlack, kQueen, 1, 5)] = BitQueen(1, 5),
  [BitLocate(kBlack, kQueen, 1, 6)] = BitQueen(1, 6),
  [BitLocate(kBlack, kQueen, 1, 7)] = BitQueen(1, 7),
  [BitLocate(kBlack, kQueen, 2, 0)] = BitQueen(2, 0),
  [BitLocate(kBlack, kQueen, 2, 1)] = BitQueen(2, 1),
  [BitLocate(kBlack, kQueen, 2, 2)] = BitQueen(2, 2),
  [BitLocate(kBlack, kQueen, 2, 3)] = BitQueen(2, 3),
  [BitLocate(kBlack, kQueen, 2, 4)] = BitQueen(2, 4),
  [BitLocate(kBlack, kQueen, 2, 5)] = BitQueen(2, 5),
  [BitLocate(kBlack, kQueen, 2, 6)] = BitQueen(2, 6),
  [BitLocate(kBlack, kQueen, 2, 7)] = BitQueen(2, 7),
  [BitLocate(kBlack, kQueen, 3, 0)] = BitQueen(3, 0),
  [BitLocate(kBlack, kQueen, 3, 1)] = BitQueen(3, 1),
  [BitLocate(kBlack, kQueen, 3, 2)] = BitQueen(3, 2),
  [BitLocate(kBlack, kQueen, 3, 3)] = BitQueen(3, 3),
  [BitLocate(kBlack, kQueen, 3, 4)] = BitQueen(3, 4),
  [BitLocate(kBlack, kQueen, 3, 5)] = BitQueen(3, 5),
  [BitLocate(kBlack, kQueen, 3, 6)] = BitQueen(3, 6),
  [BitLocate(kBlack, kQueen, 3, 7)] = BitQueen(3, 7),
  [BitLocate(kBlack, kQueen, 4, 0)] = BitQueen(4, 0),
  [BitLocate(kBlack, kQueen, 4, 1)] = BitQueen(4, 1),
  [BitLocate(kBlack, kQueen, 4, 2)] = BitQueen(4, 2),
  [BitLocate(kBlack, kQueen, 4, 3)] = BitQueen(4, 3),
  [BitLocate(kBlack, kQueen, 4, 4)] = BitQueen(4, 4),
  [BitLocate(kBlack, kQueen, 4, 5)] = BitQueen(4, 5),
  [BitLocate(kBlack, kQueen, 4, 6)] = BitQueen(4, 6),
  [BitLocate(kBlack, kQueen, 4, 7)] = BitQueen(4, 7),
  [BitLocate(kBlack, kQueen, 5, 0)] = BitQueen(5, 0),
  [BitLocate(kBlack, kQueen, 5, 1)] = BitQueen(5, 1),
  [BitLocate(kBlack, kQueen, 5, 2)] = BitQueen(5, 2),
  [BitLocate(kBlack, kQueen, 5, 3)] = BitQueen(5, 3),
  [BitLocate(kBlack, kQueen, 5, 4)] = BitQueen(5, 4),
  [BitLocate(kBlack, kQueen, 5, 5)] = BitQueen(5, 5),
  [BitLocate(kBlack, kQueen, 5, 6)] = BitQueen(5, 6),
  [BitLocate(kBlack, kQueen, 5, 7)] = BitQueen(5, 7),
  [BitLocate(kBlack, kQueen, 6, 0)] = BitQueen(6, 0),
  [BitLocate(kBlack, kQueen, 6, 1)] = BitQueen(6, 1),
  [BitLocate(kBlack, kQueen, 6, 2)] = BitQueen(6, 2),
  [BitLocate(kBlack, kQueen, 6, 3)] = BitQueen(6, 3),
  [BitLocate(kBlack, kQueen, 6, 4)] = BitQueen(6, 4),
  [BitLocate(kBlack, kQueen, 6, 5)] = BitQueen(6, 5),
  [BitLocate(kBlack, kQueen, 6, 6)] = BitQueen(6, 6),
  [BitLocate(kBlack, kQueen, 6, 7)] = BitQueen(6, 7),
  [BitLocate(kBlack, kQueen, 7, 0)] = BitQueen(7, 0),
  [BitLocate(kBlack, kQueen, 7, 1)] = BitQueen(7, 1),
  [BitLocate(kBlack, kQueen, 7, 2)] = BitQueen(7, 2),
  [BitLocate(kBlack, kQueen, 7, 3)] = BitQueen(7, 3),
  [BitLocate(kBlack, kQueen, 7, 4)] = BitQueen(7, 4),
  [BitLocate(kBlack, kQueen, 7, 5)] = BitQueen(7, 5),
  [BitLocate(kBlack, kQueen, 7, 6)] = BitQueen(7, 6),
  [BitLocate(kBlack, kQueen, 7, 7)] = BitQueen(7, 7),
  [BitLocate(kWhite, kKing, 0, 0)] = BitKing(0, 0),
  [BitLocate(kWhite, kKing, 0, 1)] = BitKing(0, 1),
  [BitLocate(kWhite, kKing, 0, 2)] = BitKing(0, 2),
  [BitLocate(kWhite, kKing, 0, 3)] = BitKing(0, 3),
  [BitLocate(kWhite, kKing, 0, 4)] = BitKing(0, 4),
  [BitLocate(kWhite, kKing, 0, 5)] = BitKing(0, 5),
  [BitLocate(kWhite, kKing, 0, 6)] = BitKing(0, 6),
  [BitLocate(kWhite, kKing, 0, 7)] = BitKing(0, 7),
  [BitLocate(kWhite, kKing, 1, 0)] = BitKing(1, 0),
  [BitLocate(kWhite, kKing, 1, 1)] = BitKing(1, 1),
  [BitLocate(kWhite, kKing, 1, 2)] = BitKing(1, 2),
  [BitLocate(kWhite, kKing, 1, 3)] = BitKing(1, 3),
  [BitLocate(kWhite, kKing, 1, 4)] = BitKing(1, 4),
  [BitLocate(kWhite, kKing, 1, 5)] = BitKing(1, 5),
  [BitLocate(kWhite, kKing, 1, 6)] = BitKing(1, 6),
  [BitLocate(kWhite, kKing, 1, 7)] = BitKing(1, 7),
  [BitLocate(kWhite, kKing, 2, 0)] = BitKing(2, 0),
  [BitLocate(kWhite, kKing, 2, 1)] = BitKing(2, 1),
  [BitLocate(kWhite, kKing, 2, 2)] = BitKing(2, 2),
  [BitLocate(kWhite, kKing, 2, 3)] = BitKing(2, 3),
  [BitLocate(kWhite, kKing, 2, 4)] = BitKing(2, 4),
  [BitLocate(kWhite, kKing, 2, 5)] = BitKing(2, 5),
  [BitLocate(kWhite, kKing, 2, 6)] = BitKing(2, 6),
  [BitLocate(kWhite, kKing, 2, 7)] = BitKing(2, 7),
  [BitLocate(kWhite, kKing, 3, 0)] = BitKing(3, 0),
  [BitLocate(kWhite, kKing, 3, 1)] = BitKing(3, 1),
  [BitLocate(kWhite, kKing, 3, 2)] = BitKing(3, 2),
  [BitLocate(kWhite, kKing, 3, 3)] = BitKing(3, 3),
  [BitLocate(kWhite, kKing, 3, 4)] = BitKing(3, 4),
  [BitLocate(kWhite, kKing, 3, 5)] = BitKing(3, 5),
  [BitLocate(kWhite, kKing, 3, 6)] = BitKing(3, 6),
  [BitLocate(kWhite, kKing, 3, 7)] = BitKing(3, 7),
  [BitLocate(kWhite, kKing, 4, 0)] = BitKing(4, 0),
  [BitLocate(kWhite, kKing, 4, 1)] = BitKing(4, 1),
  [BitLocate(kWhite, kKing, 4, 2)] = BitKing(4, 2),
  [BitLocate(kWhite, kKing, 4, 3)] = BitKing(4, 3),
  [BitLocate(kWhite, kKing, 4, 4)] = BitKing(4, 4),
  [BitLocate(kWhite, kKing, 4, 5)] = BitKing(4, 5),
  [BitLocate(kWhite, kKing, 4, 6)] = BitKing(4, 6),
  [BitLocate(kWhite, kKing, 4, 7)] = BitKing(4, 7),
  [BitLocate(kWhite, kKing, 5, 0)] = BitKing(5, 0),
  [BitLocate(kWhite, kKing, 5, 1)] = BitKing(5, 1),
  [BitLocate(kWhite, kKing, 5, 2)] = BitKing(5, 2),
  [BitLocate(kWhite, kKing, 5, 3)] = BitKing(5, 3),
  [BitLocate(kWhite, kKing, 5, 4)] = BitKing(5, 4),
  [BitLocate(kWhite, kKing, 5, 5)] = BitKing(5, 5),
  [BitLocate(kWhite, kKing, 5, 6)] = BitKing(5, 6),
  [BitLocate(kWhite, kKing, 5, 7)] = BitKing(5, 7),
  [BitLocate(kWhite, kKing, 6, 0)] = BitKing(6, 0),
  [BitLocate(kWhite, kKing, 6, 1)] = BitKing(6, 1),
  [BitLocate(kWhite, kKing, 6, 2)] = BitKing(6, 2),
  [BitLocate(kWhite, kKing, 6, 3)] = BitKing(6, 3),
  [BitLocate(kWhite, kKing, 6, 4)] = BitKing(6, 4),
  [BitLocate(kWhite, kKing, 6, 5)] = BitKing(6, 5),
  [BitLocate(kWhite, kKing, 6, 6)] = BitKing(6, 6),
  [BitLocate(kWhite, kKing, 6, 7)] = BitKing(6, 7),
  [BitLocate(kWhite, kKing, 7, 0)] = BitKing(7, 0),
  [BitLocate(kWhite, kKing, 7, 1)] = BitKing(7, 1),
  [BitLocate(kWhite, kKing, 7, 2)] = BitKing(7, 2),
  [BitLocate(kWhite, kKing, 7, 3)] = BitKing(7, 3),
  [BitLocate(kWhite, kKing, 7, 4)] = BitKing(7, 4),
  [BitLocate(kWhite, kKing, 7, 5)] = BitKing(7, 5),
  [BitLocate(kWhite, kKing, 7, 6)] = BitKing(7, 6),
  [BitLocate(kWhite, kKing, 7, 7)] = BitKing(7, 7),
  [BitLocate(kBlack, kKing, 0, 0)] = BitKing(0, 0),
  [BitLocate(kBlack, kKing, 0, 1)] = BitKing(0, 1),
  [BitLocate(kBlack, kKing, 0, 2)] = BitKing(0, 2),
  [BitLocate(kBlack, kKing, 0, 3)] = BitKing(0, 3),
  [BitLocate(kBlack, kKing, 0, 4)] = BitKing(0, 4),
  [BitLocate(kBlack, kKing, 0, 5)] = BitKing(0, 5),
  [BitLocate(kBlack, kKing, 0, 6)] = BitKing(0, 6),
  [BitLocate(kBlack, kKing, 0, 7)] = BitKing(0, 7),
  [BitLocate(kBlack, kKing, 1, 0)] = BitKing(1, 0),
  [BitLocate(kBlack, kKing, 1, 1)] = BitKing(1, 1),
  [BitLocate(kBlack, kKing, 1, 2)] = BitKing(1, 2),
  [BitLocate(kBlack, kKing, 1, 3)] = BitKing(1, 3),
  [BitLocate(kBlack, kKing, 1, 4)] = BitKing(1, 4),
  [BitLocate(kBlack, kKing, 1, 5)] = BitKing(1, 5),
  [BitLocate(kBlack, kKing, 1, 6)] = BitKing(1, 6),
  [BitLocate(kBlack, kKing, 1, 7)] = BitKing(1, 7),
  [BitLocate(kBlack, kKing, 2, 0)] = BitKing(2, 0),
  [BitLocate(kBlack, kKing, 2, 1)] = BitKing(2, 1),
  [BitLocate(kBlack, kKing, 2, 2)] = BitKing(2, 2),
  [BitLocate(kBlack, kKing, 2, 3)] = BitKing(2, 3),
  [BitLocate(kBlack, kKing, 2, 4)] = BitKing(2, 4),
  [BitLocate(kBlack, kKing, 2, 5)] = BitKing(2, 5),
  [BitLocate(kBlack, kKing, 2, 6)] = BitKing(2, 6),
  [BitLocate(kBlack, kKing, 2, 7)] = BitKing(2, 7),
  [BitLocate(kBlack, kKing, 3, 0)] = BitKing(3, 0),
  [BitLocate(kBlack, kKing, 3, 1)] = BitKing(3, 1),
  [BitLocate(kBlack, kKing, 3, 2)] = BitKing(3, 2),
  [BitLocate(kBlack, kKing, 3, 3)] = BitKing(3, 3),
  [BitLocate(kBlack, kKing, 3, 4)] = BitKing(3, 4),
  [BitLocate(kBlack, kKing, 3, 5)] = BitKing(3, 5),
  [BitLocate(kBlack, kKing, 3, 6)] = BitKing(3, 6),
  [BitLocate(kBlack, kKing, 3, 7)] = BitKing(3, 7),
  [BitLocate(kBlack, kKing, 4, 0)] = BitKing(4, 0),
  [BitLocate(kBlack, kKing, 4, 1)] = BitKing(4, 1),
  [BitLocate(kBlack, kKing, 4, 2)] = BitKing(4, 2),
  [BitLocate(kBlack, kKing, 4, 3)] = BitKing(4, 3),
  [BitLocate(kBlack, kKing, 4, 4)] = BitKing(4, 4),
  [BitLocate(kBlack, kKing, 4, 5)] = BitKing(4, 5),
  [BitLocate(kBlack, kKing, 4, 6)] = BitKing(4, 6),
  [BitLocate(kBlack, kKing, 4, 7)] = BitKing(4, 7),
  [BitLocate(kBlack, kKing, 5, 0)] = BitKing(5, 0),
  [BitLocate(kBlack, kKing, 5, 1)] = BitKing(5, 1),
  [BitLocate(kBlack, kKing, 5, 2)] = BitKing(5, 2),
  [BitLocate(kBlack, kKing, 5, 3)] = BitKing(5, 3),
  [BitLocate(kBlack, kKing, 5, 4)] = BitKing(5, 4),
  [BitLocate(kBlack, kKing, 5, 5)] = BitKing(5, 5),
  [BitLocate(kBlack, kKing, 5, 6)] = BitKing(5, 6),
  [BitLocate(kBlack, kKing, 5, 7)] = BitKing(5, 7),
  [BitLocate(kBlack, kKing, 6, 0)] = BitKing(6, 0),
  [BitLocate(kBlack, kKing, 6, 1)] = BitKing(6, 1),
  [BitLocate(kBlack, kKing, 6, 2)] = BitKing(6, 2),
  [BitLocate(kBlack, kKing, 6, 3)] = BitKing(6, 3),
  [BitLocate(kBlack, kKing, 6, 4)] = BitKing(6, 4),
  [BitLocate(kBlack, kKing, 6, 5)] = BitKing(6, 5),
  [BitLocate(kBlack, kKing, 6, 6)] = BitKing(6, 6),
  [BitLocate(kBlack, kKing, 6, 7)] = BitKing(6, 7),
  [BitLocate(kBlack, kKing, 7, 0)] = BitKing(7, 0),
  [BitLocate(kBlack, kKing, 7, 1)] = BitKing(7, 1),
  [BitLocate(kBlack, kKing, 7, 2)] = BitKing(7, 2),
  [BitLocate(kBlack, kKing, 7, 3)] = BitKing(7, 3),
  [BitLocate(kBlack, kKing, 7, 4)] = BitKing(7, 4),
  [BitLocate(kBlack, kKing, 7, 5)] = BitKing(7, 5),
  [BitLocate(kBlack, kKing, 7, 6)] = BitKing(7, 6),
  [BitLocate(kBlack, kKing, 7, 7)] = BitKing(7, 7),
};

void Print(uint64_t board) {
  for (int r = 7; r >= 0; --r) {
    for (int f = 0; f < 8; ++f) {
      if ((board >> (r * 8 + f)) & 1) {
        cout << "1";
      } else {
        cout << "0";
      }
    }
    cout << endl;
  }
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  Print(BitKnight(2, 3)); cout << endl;
  // Print(BitKing(2, 2)); cout << endl;
  // Print(BitRook(2, 2)); cout << endl;
  // Print(BitBishop(2, 2)); cout << endl;
  // Print(BitQueen(2, 2)); cout << endl;

  // Print(BitBoard(0x8080808080808080));
  // Print(BitBoard(9259542123273814144ull));

  // for (int n = 0; n < 15; n++) {
  //   // printf("%016lx\n", kBitBackslash[n]);
  //   // Print(kBitBackslash[n]); cout << endl;
  //   Print(kBitSlash[n]); cout << endl;
  // }

  // Print(ShiftRight(kBitSlash, 1)); cout << endl;
  // Print(ShiftRight(kBitSlash, 2)); cout << endl;
  // Print(ShiftRight(kBitSlash, 3)); cout << endl;
  // Print(ShiftRight(kBitSlash, 4)); cout << endl;
  // Print(ShiftRight(kBitSlash, 5)); cout << endl;

  // uint64_t lol = kSlash;
  // Print(lol); cout << endl;
  // lol = ShiftUp(lol);
  // Print(lol); cout << endl;
  // lol = ShiftUp(lol);
  // Print(lol); cout << endl;
  // lol = ShiftUp(lol);
  // Print(lol); cout << endl;

  return 0;
}

// const uint64_t kBitMaskLeft[8] = {
//   0xffffffffffffffff,
//   0x7f7f7f7f7f7f7f7f,
//   0x3f3f3f3f3f3f3f3f,
//   0x1f1f1f1f1f1f1f1f,
//   0x0f0f0f0f0f0f0f0f,
//   0x0707070707070707,
//   0x0303030303030303,
//   0x0101010101010101,
// };

// const uint64_t kBitMaskRight[8] = {
//   0xffffffffffffffff,
//   0xfefefefefefefefe,
//   0xfcfcfcfcfcfcfcfc,
//   0xf8f8f8f8f8f8f8f8,
//   0xf0f0f0f0f0f0f0f0,
//   0xe0e0e0e0e0e0e0e0,
//   0xc0c0c0c0c0c0c0c0,
//   0x8080808080808080,
// };

// inline constexpr uint64_t ShiftUp(uint64_t board, int n=1) {
//   return board << (8 * n);
// }

// inline constexpr uint64_t ShiftDown(uint64_t board, int n=1) {
//   return board >> (8 * n);
// }

// inline constexpr uint64_t ShiftLeft(uint64_t board, int n=1) {
//   return (board << n) & kBitMaskRight[n];
// }

// inline constexpr uint64_t ShiftRight(uint64_t board, int n=1) {
//   return (board >> n) & kBitMaskLeft[n];
// }

  // auto king = BitBoard("00010000"
  //                      "00000000"
  //                      "00000000"
  //                      "00000000"
  //                      "00000000"
  //                      "00000000"
  //                      "00000000"
  //                      "00000000");

  // // auto pawn_attack = BitBoard("00000000"
  // //                             "00000000"
  // //                             "00000000"
  // //                             "00000000"
  // //                             "00000000"
  // //                             "00101000"
  // //                             "00010000"
  // //                             "00000000");

  // auto bishop_attack = BitBoard("00000000"
  //                               "00000000"
  //                               "00000001"
  //                               "00000010"
  //                               "00000100"
  //                               "10001000"
  //                               "01010000"
  //                               "00100000");

  // auto rook_attack = BitBoard("00010000"
  //                             "00010000"
  //                             "00010000"
  //                             "00010000"
  //                             "00010000"
  //                             "11111111"
  //                             "00010000"
  //                             "00010000");;

  // Print(rook_attack); cout << endl;
  // ShiftDown(&rook_attack);
  // Print(rook_attack); cout << endl;
  // ShiftDown(&rook_attack);
  // Print(rook_attack); cout << endl;
  // ShiftDown(&rook_attack);

  // cout << "king & bishop_attack: " << (king & bishop_attack).any() << endl;
  // cout << "king & rook_attack: " << (king & rook_attack).any() << endl;

  // return 0;

// void ShiftRight(BitBoard* board) {
//   static const BitBoard mask(
//     "01111111"
//     "01111111"
//     "01111111"
//     "01111111"
//     "01111111"
//     "01111111"
//     "01111111"
//     "01111111"
//   );
//   *board >>= 1;
//   *board &= mask;
// }

// void ShiftLeft(BitBoard* board) {
//   static const BitBoard mask(
//     "11111110"
//     "11111110"
//     "11111110"
//     "11111110"
//     "11111110"
//     "11111110"
//     "11111110"
//     "11111110"
//   );
//   *board <<= 1;
//   *board &= mask;
// }

// void ShiftUp(BitBoard* board) {
//   *board <<= 8;
// }

// void ShiftDown(BitBoard* board) {
//   *board >>= 8;
// }

  // const string bits = board.to_string();
  // cout << bits.substr(0 * 8, 8) << endl
  //      << bits.substr(1 * 8, 8) << endl
  //      << bits.substr(2 * 8, 8) << endl
  //      << bits.substr(3 * 8, 8) << endl
  //      << bits.substr(4 * 8, 8) << endl
  //      << bits.substr(5 * 8, 8) << endl
  //      << bits.substr(6 * 8, 8) << endl
  //      << bits.substr(7 * 8, 8) << endl;

// inline constexpr uint64_t RollRight(uint64_t board) {
//   return ((board >> 1) & kMaskLeft) | ((board & ~kMaskRight) << 7);
// }

// inline constexpr uint64_t RollLeft(uint64_t board) {
//   return ((board << 1) & kMaskRight) | ((board & ~kMaskLeft) >> 7);
// }

// const uint64_t kBitSlash_ = 0x8040201008040201;
// const uint64_t kBitSlash[16] = {
//   0,
//   ShiftLeft(kBitSlash_, 7),
//   ShiftLeft(kBitSlash_, 6),
//   ShiftLeft(kBitSlash_, 5),
//   ShiftLeft(kBitSlash_, 4),
//   ShiftLeft(kBitSlash_, 3),
//   ShiftLeft(kBitSlash_, 2),
//   ShiftLeft(kBitSlash_, 1),
//   kBitSlash_,
//   ShiftRight(kBitSlash_, 1),
//   ShiftRight(kBitSlash_, 2),
//   ShiftRight(kBitSlash_, 3),
//   ShiftRight(kBitSlash_, 4),
//   ShiftRight(kBitSlash_, 5),
//   ShiftRight(kBitSlash_, 6),
//   ShiftRight(kBitSlash_, 7),
// };

// const uint64_t kBitBackslash_ = 0x0102040810204080;
// const uint64_t kBitBackslash[16] = {
//   0,
//   ShiftLeft(kBitBackslash_, 7),
//   ShiftLeft(kBitBackslash_, 6),
//   ShiftLeft(kBitBackslash_, 5),
//   ShiftLeft(kBitBackslash_, 4),
//   ShiftLeft(kBitBackslash_, 3),
//   ShiftLeft(kBitBackslash_, 2),
//   ShiftLeft(kBitBackslash_, 1),
//   kBitBackslash_,
//   ShiftRight(kBitBackslash_, 1),
//   ShiftRight(kBitBackslash_, 2),
//   ShiftRight(kBitBackslash_, 3),
//   ShiftRight(kBitBackslash_, 4),
//   ShiftRight(kBitBackslash_, 5),
//   ShiftRight(kBitBackslash_, 6),
//   ShiftRight(kBitBackslash_, 7),
// };

// const uint64_t kBitSlash = R8(00000001_bin,
//                               00000010_bin,
//                               00000100_bin,
//                               00001000_bin,
//                               00010000_bin,
//                               00100000_bin,
//                               01000000_bin,
//                               10000000_bin);

// const uint64_t kBitBackSlash = R8(10000000_bin,
//                                   01000000_bin,
//                                   00100000_bin,
//                                   00010000_bin,
//                                   00001000_bin,
//                                   00000100_bin,
//                                   00000010_bin,
//                                   00000001_bin);
