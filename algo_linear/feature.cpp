#include "feature.h"



namespace space {

	//=== CLASS  Feature_Piece

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

	std::string Feature_Piece::toString()
	{
		return "Feature_Piece" + 
				std::to_string(int(this->piecetype));
	}

	
	//=== CLASS  Feature_PawnRank

	// includes discounts for multiple pawns in a file
	double Feature_PawnRank::getValue(IBoard::Ptr board, Color color)
	{
		double counter = 0;
		const double discountFactor = 0.75;
		double DF = 1;
		int direction = colorToSign(color);
		for (int i = 4 *(1 + direction); (i>=0) && (i < 8) ; i-= direction) {
			std::optional<Piece> p = board->getPiece({ i, this->file });
			if (p.has_value()
				&& p.value().color == color
				&& (p.value().pieceType == PieceType::Pawn) ) {
				counter += (color == Color::White ? i : 7 - i) * DF;
				DF *= discountFactor;
			}
		}
		return counter;
	}

	std::string Feature_PawnRank::toString()
	{
		return "Feature_PawnRank" + 
			std::to_string(this->file);
	}


	//=== CLASS  Feature_PassedPawn

	// number of pawns of color, which are beyond any pawns of opposite color, 
	// ie. no more pawns in front in same or adjacent file
	double Feature_PassedPawn::getValue(IBoard::Ptr board, Color color)
	{
		double counter = 0;
		for(int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> p = board->getPiece({ i, j });
				if (p.has_value()
					&& p.value().color == color
					&& (p.value().pieceType == PieceType::Pawn) )
				{
					int flag = 1;
					int direction = colorToSign(color);
					for (int k = i + direction; (k > 0) && (k < 8); k+= direction)
						for (int l = j - 1; l <= j + 1; l++) 
						{
							if (l < 0 || l >= 8) {
								break;
							}
							std::optional<Piece> pOpp = board->getPiece({ k, l });
							if (p.has_value()
								&& p.value().color != color
								&& (p.value().pieceType == PieceType::Pawn)) 
							{
								flag = 0;
							}
						}
					counter += flag;
				}
			}
		return counter;
	}

	std::string Feature_PassedPawn::toString()
	{
		return std::string("Feature_PassedPawn");
	}


	//=== CLASS  Feature_MinorBalance


	double Feature_MinorBalance::getValue(IBoard::Ptr board, Color color)
	{
		int pawnCounter = 0, bishopCounter = 0, knightCounter = 0;
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> p = board->getPiece({ i,j });
				if (p.has_value())
				{
					if (p.value().pieceType == PieceType::Pawn)
						++pawnCounter;
					else if (p.value().color == color) 
					{
						if (p.value().pieceType == PieceType::Bishop)
							++bishopCounter;
						else if (p.value().pieceType == PieceType::Knight)
							++knightCounter;
					}
				}	
			}
		return (knightCounter - bishopCounter) * (pawnCounter-8);
	}

	std::string Feature_MinorBalance::toString()
	{
		return std::string("Feature_MinorBalance");
	}


	//=== CLASS  Feature_Dummy

	double Feature_Dummy::getValue(IBoard::Ptr board, Color color)
	{
		int pieceCounter = 0, kingRank = 0;
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> p = board->getPiece({ i,j });
				if (p.has_value())
				{
					++pieceCounter;
					if (p.value().color == Color::White &&
						p.value().pieceType == PieceType::King)
						kingRank = i;
				}
			}
		return (10.0 - pieceCounter) * (color == Color::White ? kingRank : 7 - kingRank);
	}

	std::string Feature_Dummy::toString()
	{
		return std::string("Feature_Dummy");
	}


	//=== CLASS  Feature_MoveCount


	double Feature_MoveCount::getValue(IBoard::Ptr board, Color color)
	{
		if (board->whoPlaysNext() == color)
			return (board->getValidMoves().size());
		return 0.0;
	}

	std::string Feature_MoveCount::toString()
	{
		return std::string("Feature_MoveCount");
	}



	//=== CLASS  Feature_PieceMove


	double Feature_PieceMove::getValue(IBoard::Ptr board, Color color)
	{
		return 0.0; // TODO
	}

	std::string Feature_PieceMove::toString()
	{
		return std::string("Feature_PieceMove");
	}



	//=== CLASS FEATURE comparison
	/*
	bool Feature::operator<(const Feature& that) const
	{
		return this;  //TODO
	}

	*/
}
