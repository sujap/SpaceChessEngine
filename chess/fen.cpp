#include "fen.h"

#include <stdexcept>
#include <sstream>

namespace {

	char pieceToChar(space::Piece piece)
	{
		char result;
		// Pawn, EnPessantCapturablePawn, Rook, Knight, Bishop, Queen, King, None
		switch (piece.pieceType)
		{
		case space::PieceType::Pawn:
			result = 'P';
			break;
		case space::PieceType::EnPassantCapturablePawn:
			result = 'P';
			break;
		case space::PieceType::Rook:
			result = 'R';
			break;
		case space::PieceType::Knight:
			result = 'N';
			break;
		case space::PieceType::Bishop:
			result = 'B';
			break;
		case space::PieceType::Queen:
			result = 'Q';
			break;
		case space::PieceType::King:
			result = 'K';
			break;
		default:
			throw std::runtime_error("Unpexpected piece type " + std::to_string(static_cast<int>(piece.pieceType)));
		}
		if (piece.color == space::Color::Black)
			result = (result + ('a' - 'A'));
		return result;
	}

} // end anonymous namespace




namespace space {
	Fen Fen::fromBoard(const IBoard::Ptr& board, int halfMoveClock, int fullMoves)
	{
		std::stringstream result;
		std::optional<Position> enPessantPosition;
		for (int rank = 7; rank >= 0; --rank)
		{
			Position pos(rank, 1);
			int skip = 0;
			for (int file = 0; file < 8; ++file)
			{
				pos.file = file;
				auto optPiece = board->getPiece(pos);
				if (!optPiece || optPiece->pieceType == PieceType::None) 
				{
					++skip;
				} else
				{
					if (skip > 0)
						result << skip;
					skip = 0;
					auto piece = *optPiece;
					result << pieceToChar(piece);

					if (piece.pieceType == PieceType::EnPassantCapturablePawn)
					{
						enPessantPosition = pos;
						if (piece.color == Color::Black)
							++(enPessantPosition->rank);
						else
							--(enPessantPosition->rank);
					}
				}
			}
			if (skip > 0) result << skip;
			if (rank > 0) result << '/';
		}

		result << ' ';
		result << (board->whoPlaysNext() == space::Color::Black ? 'b' : 'w');

		result << ' ';
		bool canCastle = false;
		if (board->canCastleLeft(Color::White)) { result << 'K'; canCastle = true; }
		if (board->canCastleRight(Color::White)) { result << 'Q'; canCastle = true; }
		if (board->canCastleRight(Color::Black)) { result << 'k'; canCastle = true; }
		if (board->canCastleLeft(Color::Black)) { result << 'q'; canCastle = true; }
		if (!canCastle) result << '-';

		result << ' ';
		if (!enPessantPosition)
			result << '-';
		else
		{
			result << ('a' + static_cast<char>(enPessantPosition->rank));
			result << enPessantPosition->file;
		}

		result << ' ' << halfMoveClock << ' ' << fullMoves;

		return result.str();
	}
}
