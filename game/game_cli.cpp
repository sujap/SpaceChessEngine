#include <iostream>
#include <string>
#include <chess/board.h>
#include <chess/board_impl.h>
#include "CliAlgo.h"

namespace {

	inline std::string getColorName(space::Color color)
	{
		switch (color)
		{
		case space::Color::White:
			return "White";
		case space::Color::Black:
			return "Black";
		default:
			throw std::runtime_error("Unrecognized color value " + std::to_string(static_cast<int>(color)));
		}
	}

	inline char pieceTypeToChar(space::PieceType pieceType)
	{
		switch (pieceType)
		{
		case space::PieceType::Pawn:
			return 'p';
		case space::PieceType::EnPassantCapturablePawn:
			return 'p';
		case space::PieceType::Rook:
			return 'r';
		case space::PieceType::Knight:
			return 'n';
		case space::PieceType::Bishop:
			return 'b';
		case space::PieceType::Queen:
			return 'q';
		case space::PieceType::King:
			return 'k';
		case space::PieceType::None:
			throw std::runtime_error("Cannot convert piece type 'None' to text");
		default:
			throw std::runtime_error("pieceType " + std::to_string(static_cast<int>(pieceType)) + " not recognized.");
		}
	}

	inline char pieceToChar(space::Piece piece) {

		char result = pieceTypeToChar(piece.pieceType);
		if (piece.color == space::Color::White)
			result = result + 'A' - 'a';
		return result;
	}

	inline space::Color getOppositeColor(space::Color color) {
		switch (color)
		{
		case space::Color::Black:
			return space::Color::White;
		case space::Color::White:
			return space::Color::Black;
		default:
			throw std::runtime_error("Unrecognized color value " + std::to_string(static_cast<int>(color)));
		}
	}

	void printBoard(std::ostream& out, const space::IBoard& board)
	{
		out << "\n------------";
		for (int rank = 0; rank < 8; ++rank)
		{
			out << (8 - rank) << "  ";
			for (int file = 0; file < 8; ++file)
			{
				auto piece = board.getPiece({ rank, file });
				if (piece.has_value())
					out << pieceToChar(piece.value()) << "  ";
				else
					out << ".  ";
			}
			out << "\n\n";
		}
		out << "   ";
		for (int file = 0; file < 8; ++file)
		{
			out << 'a' + file << "  ";
		}
		out << "\n\n";
	}
}

int main() {
	auto board = space::BoardImpl::getStartingBoard();
	auto whiteAlgo = space::CliAlgo::create(std::cin, std::cout);
	auto blackAlgo = space::CliAlgo::create(std::cin, std::cout);
	bool recursiveError = false;
	while (true)
	{
		auto algo = blackAlgo;
		if (board->whoPlaysNext() == space::Color::White)
			algo = whiteAlgo;

		try {

			if (board->isCheckMate()) {
				std::cout
					<< "Check mate!\n"
					<< getColorName(getOppositeColor(board->whoPlaysNext())) << " wins!"
					<< std::endl;
				return 0;
			}
			if (board->isStaleMate()) {
				std::cout
					<< getColorName(board->whoPlaysNext()) << " is stuck on stale mate!\n"
					<< "It's a draw!"
					<< std::endl;
				return 0;
			}

			auto nextMove = algo->getNextMove(board);
			auto validMoves = board->getValidMoves();
			auto validMoveIt = validMoves.find(nextMove);
			if (validMoveIt == validMoves.cend())
				throw std::runtime_error(getColorName(board->whoPlaysNext()) + " played invalid move.");
			board = validMoveIt->second;
		}
		catch (const std::bad_alloc&)
		{
			throw;
		}
		catch (const std::exception& e)
		{
			std::cout << "Error: " << e.what();
			if (recursiveError)
				throw;
			recursiveError = true;
		}
		recursiveError = false;
	}
	return 0;
}