// bitboard.h

#ifndef CHESSY_BITMOVE_H_
#define CHESSY_BITMOVE_H_

#include <string>
#include <utility>
#include <vector>

#include "bitboard.h"
#include "piece.h"
#include "square.h"

namespace chessy {

struct Bitmove;

typedef std::vector<Bitmove> Bitmoves;

struct Bitmove {
 public:
  static const Bitmove kInvalid;
  Bitmove()
      : source(Square::kInvalid),
        dest(Square::kInvalid) {}
  explicit Bitmove(Square source)
      : source(source),
        dest(source),
        source_bit(Bitboard(source)) {}
  Bitmove(Square source, Square dest, Bitboard path)
      : source(source),
        dest(dest),
        path(path),
        source_bit(Bitboard(source)),
        dest_bit(Bitboard(dest)) {}
  inline bool IsValid() const { return source.IsValid() && dest.IsValid(); }
  std::string ToString() const;
  Square source;
  Square dest;
  Bitboard path;
  Bitboard source_bit;
  Bitboard dest_bit;
};

void InitBitmoves();  // Generate table of all possible moves.
const Bitmove& GetBitmove(Piece piece, Square source, Square dest);
const Bitmoves& GetBitmoves(Piece piece, Square source);
const Bitboard& GetBitmovesMask(Piece piece, Square source);
std::ostream& operator<<(std::ostream& os, const Bitmove& move);

}  // namespace chessy

#endif  // CHESSY_BITMOVE_H_
