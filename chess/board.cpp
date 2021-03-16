#include "common/base.h"
#include "board.h"

#include <stdexcept>


namespace space {
	PieceType charToPieceType(char c)
	{
		// Pawn, EnPessantCapturablePawn, Rook, Knight, Bishop, Queen, King, None
		switch (c)
		{
		case 'p':
		case 'P':
			return PieceType::Pawn;
		case 'r':
		case 'R':
			return PieceType::Rook;
		case 'n':
		case 'N':
			return PieceType::Knight;
		case 'b':
		case 'B':
			return PieceType::Bishop;
		case 'q':
		case 'Q':
			return PieceType::Queen;
		case 'k':
		case 'K':
			return PieceType::King;
		default:
			throw std::runtime_error(std::string("Unrecognizable piece type '") + c + "'");
		}
	}

	char pieceTypeToChar(PieceType p) {
		switch (p)
		{
		case PieceType::Pawn:
			return 'p';
		case PieceType::EnPassantCapturablePawn:
			return 'p';
		case PieceType::Rook:
			return 'r';
		case PieceType::Knight:
			return 'n';
		case PieceType::Bishop:
			return 'b';
		case PieceType::Queen:
			return 'q';
		case PieceType::King:
			return 'k';
		case PieceType::None:
			throw std::runtime_error("Cannot convert piece type 'None' to text");
		default:
			throw std::runtime_error("pieceType " + std::to_string(static_cast<int>(p)) + " not recognized.");
		}
	}

	Piece::Piece(char c) {
		this->pieceType = charToPieceType(c);
		if (c >= 'A' && c <= 'Z') {
			this->color = Color::White;
		}
		else if (c >= 'a' && c <= 'z') {
			this->color = Color::Black;
		}
		else {
			throw std::runtime_error("Unrecognised piece " + c);
		}
	}

	char Piece::toChar() {
		char c = pieceTypeToChar(this->pieceType);
		if (this->color == Color::White)
			c += 'A' - 'a';
		return c;
	}



	// STRUCT Move

	Move::Move(std::string s) {
		//TODO


	}


	std::string Move::toString() {
		std::string result("     ");
		result[0] = static_cast<char>(this->sourceFile + 'a');
		result[1] = static_cast<char>(this->sourceRank + '1');
		result[2] = static_cast<char>(this->destinationFile + 'a');
		result[3] = static_cast<char>(this->destinationRank + '1');
		if (this->promotedPiece != space::PieceType::None)
			result[4] = pieceTypeToChar(this->promotedPiece);
		return result;
	}

	std::string moveToString(Move m, IBoard::Ptr board)
	{
		std::string result = m.toString();
		std::optional<Piece> pSource = board->getPiece({ m.sourceRank, m.sourceFile });
		std::optional<Piece> pTarget = board->getPiece({ m.destinationRank,m.destinationFile });
		space_assert(pSource.has_value(), "No piece to move " + result);
		result = pSource.value().toChar() + result;
		result += pTarget.has_value() ? pTarget.value().toChar() : '-';
		return result;
	}

	bool Move::operator<(Move const& that) const
	{
		int diff = this->sourceRank - that.sourceRank;

		if (diff == 0) {
			diff = this->sourceFile - that.sourceFile;
			if (diff == 0) {
				diff = this->destinationRank - that.destinationRank;
				if (diff == 0) {
					diff = this->destinationFile - that.destinationFile;
					if (diff == 0) {
						diff = static_cast<int>(this->promotedPiece) - static_cast<int>(that.promotedPiece);
					}
				}
			}
		}
		if (diff < 0) {
			return true;
		}
		return false;
	}
}
