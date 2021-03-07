#pragma once

#include "board.h"

#include <string>

/*
> Each rank is described, starting with rank 8 and ending with rank 1
>		Algebraic notation with White pieces in uppercase, Black in lowercase
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

	};
}
