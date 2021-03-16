#include <iostream>
#include <string>
#include <vector>

#include <chess/board.h>
#include <chess/board_impl.h>
#include <algo_linear/algoLinear.h>

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
		out << "  |  a  b  c  d  e  f  g  h  |\n"
			<< "--+--------------------------+--\n";
		for (int rank = 7; rank >= 0; --rank)
		{
			out << (rank + 1) << " |  ";
			for (int file = 0; file < 8; ++file)
			{
				auto piece = board.getPiece({ rank, file });
				if (piece.has_value())
					out << piece.value().toChar()    << "  ";
				else
					out << ".  ";
			}
			out << "| " << (rank + 1) << "\n";
			if (rank > 0)
				out << "  |                          |  \n";
		}
		out << "--+--------------------------+--\n"
			<< "  |  a  b  c  d  e  f  g  h  |\n\n";
	}
}

int main() {
	auto board = space::BoardImpl::getStartingBoard();
	auto whiteAlgo = space::CliAlgo::create(std::cin, std::cout);
	// auto blackAlgo = space::CliAlgo::create(std::cin, std::cout);
	std::vector<double> wts = { 1, 5, 4, 4, 12 };
	auto bb = space::AlgoLinearDepthTwoExt(4, wts);
	auto blackAlgo = std::make_shared< space::AlgoLinearDepthTwoExt>(bb);

	bool recursiveError = false;
	while (true)
	{
		printBoard(std::cout, *board);
		 auto algo = board->whoPlaysNext() == space::Color::White ? whiteAlgo : blackAlgo;

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