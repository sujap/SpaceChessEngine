#pragma once

#include "board.h"
#include "fen.h"

#include <array>
#include<vector>


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
		static Ptr fromFen(const Fen& fen);

	private:
		std::array<std::array<Piece, 8>, 8> m_pieces;
		bool m_canWhiteCastleLeft;
		bool m_canWhiteCastleRight;
		bool m_canBlackCastleLeft;
		bool m_canBlackCastleRight;
		Color m_whoPlaysNext;
		bool checkObstructions(Move m) const;
		bool isCheckMate(int rank, int file, bool current = true) const;
		std::vector<Move> getAllmoves(bool current = true) const;
		std::vector<Move> getAllmoves(int rank, int file, bool current = true) const;
		std::vector<Move> getAllmovesWithoutObstructions(bool current = true) const;
		inline bool inRange(int x) const { return (x >= 0) && (x < 8); }
		Color getColor(bool current = true) const;
	};
}
