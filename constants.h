// constants.h - chessy constants
// 2013.02.10

#ifndef CHESSY_CONSTANTS_H_
#define CHESSY_CONSTANTS_H_

#include <array>
#include <string>
#include <vector>

#include "chessy.h"
#include "board.h"
#include "move.h"

using std::string;
using std::array;
using std::vector;

namespace chessy {


  // Units are in centi-pawns
  const array<int, kPieceTypes> kPieceValues = {{
    100,    // Pawn
    300,    // Knight
    300,    // Bishop
    500,    // Rook
    900,    // Queen
    6666,    // King
  }};

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


}  // chessy

#endif  // CHESSY_CONSTANTS_H_
