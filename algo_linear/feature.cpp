#include "feature.h"



namespace space {


	double Feature_Piece::getValue(IBoard::Ptr board, Color color)
	{
		double counter = 0;
		for(int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> p = board->getPiece({ i,j });
				if (p.has_value())
					if (p.value().pieceType == this->piecetype && p.value().color == color)
						counter+= 1;
			}
		
		return counter;
	}

	// discounts for multiple pawns in a file
	double Feature_PawnRank::getValue(IBoard::Ptr board, Color color)
	{
		double counter = 0;
		const double discountFactor = 0.75;
		double DF = 1;
		for (int i = 0; i < 8; i++) {
			std::optional<Piece> p = board->getPiece({ i, this->file });
			if (p.has_value()
				&& p.value().color == color
				&& (p.value().pieceType == PieceType::Pawn
					|| p.value().pieceType == PieceType::EnPassantCapturablePawn)) {
				counter += (color == Color::White ? i : 7 - i) * DF;
				DF *= discountFactor;
			}
		}
		return counter;
	}

	double Feature_MoveCount::getValue(IBoard::Ptr board, Color color)
	{
		if (board->whoPlaysNext() == color)
			return (board->getValidMoves().size());
		return 0.0;
	}

	double Feature_PieceMove::getValue(IBoard::Ptr board, Color color)
	{
		return 0.0; // TODO
	}
}