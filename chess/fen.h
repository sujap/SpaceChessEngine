#pragma once

#include "board.h"

#include <string>

namespace space {
	struct Fen {
		std::string fen;
		Fen(const std::string& v_fen) : fen(v_fen) {}

		static Fen fromBoard(const IBoard::Ptr& board, int halfMoveClock, int fullMoves);

	};
}