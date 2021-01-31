#pragma once

#include "board.h"

namespace space {
	class BoardImpl : public IBoard {
	public:
		using BoardImplPtr = std::shared_ptr<BoardImpl>;
		Color getNextMove() const override;
		std::optional<Piece> getPiece(Position position) const override;
		bool canCastleLeft(Color color) const override;
		bool canCasleRight(Color color) const override;
		bool isStaleMate() const override;
		bool isCheckMate() const override;
		std::optional<Ptr> move(Move move) const override;
		std::map<Move, Ptr> getPossibleMoves() const override;
	};
}