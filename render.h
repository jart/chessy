// render.h - board rendering helpers
// 2013.02.09

#ifndef CHESSY_RENDER_H_
#define CHESSY_RENDER_H_

#include <string>

namespace chessy {

class Board;
struct Bitmove;

namespace render {

const int kSquareSize = 3;  // Set in terms of console characters.
const int kSquareCenter = kSquareSize / 2;

void Status(std::string msg);
bool YesNoPrompt(std::string msg);

// ChessyMsg by itself appends messages, but requires one of ChessyNewMsg() or
// ChessyNewMsgMirror() to have been called in the past. The second two
// functions clears either below or above the chess board, and "begin" a new
// chessy message streams.
void ChessyMsg(std::string msg);
void ChessyNewMsg(std::string msg);  // Clears Chessy's text.
void ChessyNewMsgMirror(std::string msg);

std::string HumanMovePrompt();

// UpdateBoard should be called in most cases, rather than Board::Print()
// because this redraws a mimimal number of console chars in response to a move
// update, while Board::Print draw *every* character over again.
void UpdateBoard(const Board& board, const Bitmove& move);

void Everything(const Board& board);

}  // namespace render

}  // namespace chessy

#endif  // CHESSY_RENDER_H_
