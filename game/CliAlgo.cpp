#include "CliAlgo.h"

#include <string>

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
			throw std::runtime_error(std::string("Unsupported piece type '") + ch + "'\n");
		}
	}

	void invalidMove(const std::string& message, std::ostream& out)
	{
		out << "Move should be 4 or 5 chars long.\n"
			<< "\t<source file><source rank><target file><target rank>[<pawn promotion>]\n"
			<< "E.g. to move a2 to a4\n"
			<< "\ta2a4\n"
			<< "or to promote white pawn to queen\n"
			<< "\ta7a8q."
			<< std::endl;
		throw std::runtime_error("Invalid move: " + message);
	}

	space::Move parseMove(std::string moveStr, std::ostream& out)
	{
		for (auto& ch : moveStr) ch = std::tolower(ch);
		if (moveStr.size() != 4 && moveStr.size() != 5)
			invalidMove("Move was not 4 or 5 chars long.", out);
		char srcFile = moveStr[0];
		if (srcFile < 'a' || srcFile > 'h') invalidMove(std::string("Source file '") + srcFile + "' has to be between 'a' and 'h'", out);
		char srcRank = moveStr[1];
		if (srcRank < '1' || srcRank > '8') invalidMove(std::string("Source rank '") + srcRank + "' has to be between '1' and '7'", out);
		char tgtFile = moveStr[2];
		if (tgtFile < 'a' || tgtFile > 'h') invalidMove(std::string("Target file '") + tgtFile + "' has to be between 'a' and 'h'", out);
		char tgtRank = moveStr[3];
		if (tgtRank < '1' || tgtRank > '8') invalidMove(std::string("Target rank '") + tgtRank + "' has to be between '1' and '7'", out);

		space::PieceType pieceType = space::PieceType::None;
		if (moveStr.size() == 5) pieceType = charToPieceType(moveStr[4]);

		space::Move move;
		move.sourceRank = srcRank - '1';
		move.sourceFile = srcFile - 'a';
		move.destinationRank = tgtRank - '1';
		move.destinationFile = tgtFile - 'a';
		move.promotedPiece = pieceType;

		return move;
		
	}
}


namespace space{
	Move CliAlgo::getNextMove(IBoard::Ptr board) {
		auto validMoves = board->getValidMoves();
		Move result;
		std::string moveStr;
		do {
			m_outputStream << "Enter move: ";
			m_inputStream >> moveStr;
			result = parseMove(moveStr, m_outputStream);
			if (isPawnPromotion(result, *board))
			{
				m_outputStream << "Enter the piece the pawn is promoted to: ";
				char piece;
				m_inputStream >> piece;
				result.promotedPiece = charToPieceType(piece);
			}
			if (validMoves.count(result) == 0)
			{
				m_outputStream << "Invalid move '" << result.toString() << "'. Valid moves are: \n";
				for (const auto& validMove : validMoves) {
					Move m = validMove.first;
					m_outputStream << m.toString() << " ";
				}
				m_outputStream << std::endl;
			}
		} while (validMoves.count(result) == 0);
		return result;
	}
}