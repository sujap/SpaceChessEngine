#include "common/base.h"
#include "board.h"

#include <stdexcept>


namespace space {

	int colorToSign(Color c) {
		return c == Color::White ? 1 : -1;
	}



	PieceType charToPieceType(char c)
	{
		// Pawn, Rook, Knight, Bishop, Queen, King, None
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
			return 'P';
		case PieceType::Rook:
			return 'R';
		case PieceType::Knight:
			return 'N';
		case PieceType::Bishop:
			return 'B';
		case PieceType::Queen:
			return 'Q';
		case PieceType::King:
			return 'K';
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


	// (Default: color=true) Uppercase for White, lowercase for black
	// (color = false) All Uppercase
	char Piece::as_char(bool color) const{		
		char c = pieceTypeToChar(this->pieceType);
		if (color && this->color == Color::Black)
			c += 'a' - 'A';
		return c;
	}

	std::string Piece::as_unicode() const {
		if (this->color == Color::White) {
			switch (this->pieceType) {
			case PieceType::King: return "\u2654";
			case PieceType::Queen: return "\u2655";
			case PieceType::Rook: return "\u2656";
			case PieceType::Bishop: return "\u2657";
			case PieceType::Knight: return "\u2658";
			case PieceType::Pawn: return "\u2659";
			}
		}
		else {
			switch (this->pieceType) {
			case PieceType::King: return "\u265a";
			case PieceType::Queen: return "\u265b";
			case PieceType::Rook: return "\u265c";
			case PieceType::Bishop: return "\u265d";
			case PieceType::Knight: return "\u265e";
			case PieceType::Pawn: return "\u265f";
			}
		}

		// PieceType::None
		return " ";
	}

	// STRUCT Move

	Move::Move(const std::string& s) {
		//TODO


	}


	std::string Move::toString() const{
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
		result = pSource.value().as_char() + result;
		result += pTarget.has_value() ? pTarget.value().as_char() : '-';
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
