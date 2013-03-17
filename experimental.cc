#include <cstdio>
#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "square.h"

using namespace std;
using namespace chessy;

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();

  // Square sq = Square(0, 0) + Square(1, 1) + Square(0, 5);
  // printf("%x rank=%d file=%d valid=%d\n",
  //        sq.x88(), sq.rank(), sq.file(), sq.valid());
  // return 0;

  // Print(BitKnight(2, 3)); cout << endl;
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

// static const uint64_t kBitRow[8] = {
//   0x00000000000000ff,
//   0x000000000000ff00,
//   0x0000000000ff0000,
//   0x00000000ff000000,
//   0x000000ff00000000,
//   0x0000ff0000000000,
//   0x00ff000000000000,
//   0xff00000000000000,
// };

// static const uint64_t kBitColumn[8] = {
//   0x0101010101010101,
//   0x0202020202020202,
//   0x0404040404040404,
//   0x0808080808080808,
//   0x1010101010101010,
//   0x2020202020202020,
//   0x4040404040404040,
//   0x8080808080808080,
// };

// static const uint64_t kBitSlash[15] = {
//   0x0000000000000001,
//   0x0000000000000102,
//   0x0000000000010204,
//   0x0000000001020408,
//   0x0000000102040810,
//   0x0000010204081020,
//   0x0001020408102040,
//   0x0102040810204080,
//   0x0204081020408000,
//   0x0408102040800000,
//   0x0810204080000000,
//   0x1020408000000000,
//   0x2040800000000000,
//   0x4080000000000000,
//   0x8000000000000000,
// };

// static const uint64_t kBitBackslash[15] = {
//   0x0100000000000000,
//   0x0201000000000000,
//   0x0402010000000000,
//   0x0804020100000000,
//   0x1008040201000000,
//   0x2010080402010000,
//   0x4020100804020100,
//   0x8040201008040201,
//   0x0080402010080402,
//   0x0000804020100804,
//   0x0000008040201008,
//   0x0000000080402010,
//   0x0000000000804020,
//   0x0000000000008040,
//   0x0000000000000080,
// };

// constexpr uint64_t BitBit(int rank, int file) {
//   return (rank < 8 && file < 8) ? 1ull << (rank * 8) << file : 0;
// }

// constexpr uint64_t BitRook(int rank, int file) {
//   return kBitRow[rank] | kBitColumn[file];
// }

// constexpr uint64_t BitBishop(int rank, int file) {
//   return kBitSlash[rank + file] | kBitBackslash[rank - file + 7];
// }

// constexpr uint64_t BitQueen(int rank, int file) {
//   return BitRook(rank, file) | BitBishop(rank, file);
// }

// constexpr int Abs(int x) {
//   return (x > 0) ? x : 0;
// }

// constexpr uint64_t BitKing(int rank, int file) {
//   return ((file > 0)
//           ? ((7 << (file - 1) << (Abs(rank - 1) * 8)) |
//              (7 << (file - 1) << (rank * 8)) |
//              (7 << (file - 1) << ((rank + 1) * 8)))
//           : ((3 << (Abs(rank - 1) * 8)) |
//              (3 << (rank * 8)) |
//              (3 << ((rank + 1) * 8))));
// }

// constexpr uint64_t BitPawn(int rank, int file, Color color) {
//   return (BitBit(rank, file) |
//           ((color == kWhite)
//            ? (BitBit(rank + 1, file - 1) | BitBit(rank + 1, file + 1))
//            : (BitBit(rank - 1, file - 1) | BitBit(rank - 1, file + 1))));
// }

// constexpr uint64_t BitKnight(int rank, int file) {
//   return (BitBit(rank, file) |
//           BitBit(rank + 2, file - 1) |
//           BitBit(rank + 2, file + 1) |
//           BitBit(rank - 2, file + 1) |
//           BitBit(rank - 2, file - 1) |
//           BitBit(rank + 1, file + 2) |
//           BitBit(rank + 1, file - 2) |
//           BitBit(rank - 1, file + 2) |
//           BitBit(rank - 1, file - 2));
// }

// constexpr size_t BitLocate(Color color, Piece piece, int rank, int file) {
//   return ((static_cast<size_t>(file)  << 0) |  // 3 bits
//           (static_cast<size_t>(rank)  << 3) |  // 3 bits
//           (static_cast<size_t>(piece) << 6) |  // 3 bits
//           (static_cast<size_t>(color) << 9));  // 2 bits
// }

// // for piece in ['Pawn', 'Knight', 'Bishop', 'Rook', 'Queen', 'King']:
// //     for color in ['kWhite', 'kBlack']:
// //         for rank in range(8):
// //             for file_ in range(8):
// //                 extra = ''
// //                 if piece == 'Pawn':
// //                     extra = ', ' + color
// //                 print '  [BitLocate(%s, k%s, %d, %d)] = Bit%s(%d, %d%s),' % (
// //                     color, piece, rank, file_, piece, rank, file_, extra)

// static uint64_t BitStatic[] = {
// };

// void Print(uint64_t board) {
//   for (int r = 7; r >= 0; --r) {
//     for (int f = 0; f < 8; ++f) {
//       if ((board >> (r * 8 + f)) & 1) {
//         cout << "1";
//       } else {
//         cout << "0";
//       }
//     }
//     cout << endl;
//   }
// }

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
