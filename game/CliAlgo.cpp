#include "CliAlgo.h"


namespace {
	inline bool isPawnPromotion(const space::Move & move, const space::IBoard & board) {
		using namespace space;
		auto piece = board.getPiece({ move.sourceRank, move.sourceFile });
		if (!piece || piece->pieceType != PieceType::Pawn)
			return false;
		else if (board.whoPlaysNext() == Color::Black
			&& move.destinationRank == 0)
		{
			return true;
		}
		else if (board.whoPlaysNext() == Color::White
			&& move.destinationRank == 7)
		{
			return true;
		}
		else
			return false;
	}

	inline space::PieceType charToPieceType(char ch)
	{
		using namespace space;
		if (ch >= 'A' && ch <= 'Z')
			ch = ch + 'a' - 'A';
		switch (ch)
		{
		case 'q':
			return PieceType::Queen;
		case 'n':
			return PieceType::Knight;
		case 'r':
			return PieceType::Rook;
		case 'b':
			return PieceType::Bishop;
		default:
			throw std::runtime_error(std::string("Unsupported piece type '") + ch + "'");
		}
	}
}


namespace space{
	Move CliAlgo::getNextMove(IBoard::Ptr board) {
		auto validMoves = board->getValidMoves();
		Move result;
		do {
			m_outputStream << "Enter sourceRank sourceFile targetRank targetFile: ";
			m_inputStream
				>> result.sourceRank
				>> result.sourceFile
				>> result.destinationRank
				>> result.destinationFile;
			if (isPawnPromotion(result, *board))
			{
				m_outputStream << "Enter the piece the pawn is promoted to: ";
				char piece;
				m_inputStream >> piece;
				result.promotedPiece = charToPieceType(piece);
			}
			if (validMoves.count(result) == 0)
				m_outputStream << "Invalid move.\n";
		} while (validMoves.count(result) == 0);
		return result;
	}
}