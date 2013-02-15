// term.h - ansi terminal routines
// 2013.02.09

#ifndef CHESSY_TERM_H_
#define CHESSY_TERM_H_

#include <string>

#include "square.h"
#include "render.h"

namespace chessy {
namespace term {

// TODO: Make these conversion thingies better.
std::string i2s(int x);
std::string x2s(int x);

// ANSI escape sequences for console drawing shenanigans.
// 256 reference: http://www.pixelbeat.org/docs/terminal_colours/
const std::string kX = "\x1b[";

// Visibility
const std::string kHideCursor    = kX + "?25l";
const std::string kShowCursor    = kX + "?25h";
const std::string kSaveCursor    = kX + "s";
const std::string kRestoreCursor = kX + "u";

const std::string kClear      = kX + "2J" + kX + "H";  // Clear ALL
const std::string kClearEnd   = kX + "0J";  // only from cursor to end
const std::string kClearStart = kX + "1J";  // only from cursor to start

// Position/Offsets
const int kBoardStart  = 12;  // Rows from the top
const int kBoardLeft   = 4;
const int kBoardPad    = 2;  // For the Rank numbers
const int kBoardHeight = 8 * render::kSquareSize;
const int kBottom_ = kBoardStart + kBoardHeight + 7;

// Cursor Jumping
const std::string kBoardPosition  = kX + i2s(kBoardStart) + ";1H";
const std::string kInputPosition  = kX + i2s(kBottom_) + ";8H";
const std::string kStatusPosition = kX + i2s(kBottom_ - 2) + ";4H";
const std::string kChessyPosition = kX + "4;4H";
const std::string kBoardMargin = std::string(kBoardLeft - 1, ' ');

const std::string kDown = kX + "1B";
const std::string kBack = kX + "1D";
const std::string kBackSquare = kX + i2s(render::kSquareSize * 2) + "D";

// Color
const std::string kReset = kX + "0m";
const std::string kBold = kX + "38;5;255;0;0m";
const std::string kFG = kX + "38;5;";
const std::string kBG = kX + "48;5;";

// Foreground/text
const std::string kWhite        = kX  + "37m";
const std::string kBlack        = kX  + "30m";
const std::string kGray         = kFG + "241m";
const std::string kPurple       = kFG + "55m";
const std::string kRed          = kFG + "196m";
const std::string kPink         = kFG + "213m";
const std::string kRose         = kFG + "212m";
const std::string kOrange       = kFG + "203m";
const std::string kYellow       = kFG + "226m";
const std::string kGreen        = kFG + "46m";
const std::string kBlue         = kFG + "14m";
const std::string kNavy         = kFG + "61m";

// Background
const std::string kWhiteBg      = kX  + "47m";
const std::string kGrayBg       = kBG + "238m";
const std::string kBlackBg      = kBG + "234m";
const std::string kLightGrayBg  = kBG + "242m";
const std::string kLightBlackBg = kBG + "235m";
const std::string kRedBg        = kBG + "196m";
const std::string kPinkBg       = kBG + "182m";
const std::string kOrangeBg     = kBG + "203m";
const std::string kBlueBg       = kBG + "61m";
const std::string kDawnBg       = kBG + "59m";
const std::string kDuskBg       = kBG + "60m";

// Application
const std::string kWhitePiece = kPink;
const std::string kWhiteSquare = kGrayBg;
const std::string kWhiteSquareActive = kDawnBg;

const std::string kBlackPiece = kBlue;
const std::string kBlackSquare = kBlackBg;
const std::string kBlackSquareActive = kDuskBg;

const std::string kStatusColor  = kRed;
const std::string kChessyPrompt = kRose + "Chessy: ";
const std::string kChessyColor  = kWhite;
const std::string kChessy       = kChessyPrompt + kChessyColor + kSaveCursor;

const std::string kClearAboveBoard = kBoardPosition  + kClearStart;
const std::string kClearBelowBoard = kStatusPosition + kClearEnd;

}  // namespace term
}  // namespace chessy

#endif  // CHESSY_TERM_H_
