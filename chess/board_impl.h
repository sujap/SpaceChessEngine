#pragma once

#include "board.h"

namespace space {

	class BoardImpl : public IBoard {
	public:
		using BoardImplPtr = std::shared_ptr<BoardImpl>;
		Color whoPlaysNext() const override;
		std::optional<Piece> getPiece(Position position) const override;
		bool canCastleLeft(Color color) const override;
		bool canCastleRight(Color color) const override;
		bool isStaleMate() const override;
		bool isCheckMate() const override;
		std::optional<Ptr> updateBoard(Move move) const override;
		std::map<Move, Ptr> getPossibleMoves() const override;

		static Ptr getStartingBoard();

	private:
		Piece m_pieces[8][8];
		bool m_canWhiteCastleLeft;
		bool m_canWhiteCastleRight;
		bool m_canBlackCastleLeft;
		bool m_canBlackCastleRight;
		Color m_whoPlaysNext;
		bool isLegalMove(Move m);
		std::map<Move, Ptr>getAllmoves(int rank, int file);
		inline bool inRange(int x) { return (x >= 0) && (x < 8); }
	};
}
