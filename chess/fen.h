#pragma once

#include "board.h"

#include <string>
#include <vector>

/*
> Each rank is described, starting with rank 8 and ending with rank 1
>		Algebraic notation with White pieces in Uppercase, Black in lowercase
> Active color
> Castling availability
> En passant target square or -
> Halfmove clock (since last capture or pawn advance)
> Fullmove number 
*/

namespace space {
	struct Fen {
		std::string fen;
		Fen(const std::string& v_fen) : fen(v_fen) {}

		static Fen fromBoard(const IBoard::Ptr& board, int halfMoveClock, int fullMoves);

		static std::vector<std::string> moves2string(const IBoard::Ptr& board, std::vector<Move> movesList);

		static std::vector<std::string> moves2string(const IBoard::Ptr& board, IBoard::MoveMap mv);

		static std::string moveStr(const IBoard::Ptr& board, Move m);

	private:

	};
}
