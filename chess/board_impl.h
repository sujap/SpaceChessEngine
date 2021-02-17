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
		bool m_WhiteKingNotMoved;
		bool m_WhiteLeftRookNotMoved;
		bool m_WhiteRightRookNotMoved;
		bool m_BlackKingNotMoved;
		bool m_BlackLeftRookNotMoved;
		bool m_BlackRightRookNotMoved;
		Color m_whoPlaysNext;
		bool checkObstructions(Move m);
		std::map<Move, Ptr>getAllmoves() const;
		std::map<Move, Ptr>getAllmoves(int rank, int file) const;
		inline bool inRange(int x) const { return (x >= 0) && (x < 8); }
	};
}
