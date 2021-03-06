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
		MoveMap getValidMoves() const override;

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
		bool isUnderCheck(Color color, std::optional<Position> targetKingPosition = std::nullopt) const;
		std::vector<Move> getAllMoves(Color color) const;
		std::vector<Move> getAllMoves(Position position) const;
		std::vector<Move> getAllmovesWithoutObstructions(Color color) const;
		inline bool inRange(int x) const { return (x >= 0) && (x < 8); }
		Color getColor(bool current = true) const;
	};
}
