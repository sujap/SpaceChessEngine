#include <iostream>
#include <string>
#include <vector>

#include <chess/board.h>
#include <chess/board_impl.h>
#include <chess/algo_factory.h>
#include <algo_linear/algoLinear.h>

#include "CliAlgo.h"

namespace {


	static std::string WhiteAlgoFieldName = "WhiteAlgo";
	static std::string BlackAlgoFieldName = "BlackAlgo";

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

		out << "  |  a  b  c  d  e  f  g  h  |\n"
			<< "--+--------------------------+--\n";
		for (int rank = 7; rank >= 0; --rank)
		{
			out << (rank + 1) << " |  ";
			for (int file = 0; file < 8; ++file)
			{
				auto piece = board.getPiece({ rank, file });
				if (piece.has_value())
					out << pieceToChar(piece.value()) << "  ";
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

	nlohmann::json parseConfig(int argc, char const* const* const argv)
	{
		nlohmann::json result;
		for (int iarg = 1; iarg < argc; ++iarg)
		{
			std::string arg = argv[iarg];
			if (arg == "--configFile")
			{
				if (++iarg >= argc)
					throw std::runtime_error("invalid command line arguments: expected filename after '--configFile'");
				std::ifstream fin(argv[iarg]);
				fin >> result;
			}
			else if (arg == "--blackAlgo")
			{
				if (++iarg >= argc)
					throw std::runtime_error("invalid command line arguments: expected filename after '--blackAlgo'");
				result[BlackAlgoFieldName] = argv[iarg];
			}
			else if (arg == "--whiteAlgo")
			{
				if (++iarg >= argc)
					throw std::runtime_error("invalid command line arguments: expected filename after '--whiteAlgo'");
				result[WhiteAlgoFieldName] = argv[iarg];
			}
			else if (arg == "--help" || arg == "-h")
			{
				std::cout << "Space chess command line game engine.\n\t"
					<< argv[0] << " [--configFile <json config file>] [--blackAlgo <blackAlgoName>] [--whiteAlgo <whiteAlgoName>] [--help|-h]"
					<< std::endl;
				std::exit(0);
			}

		}
		return result;
	}
}

int main(int argc, char const * const * const argv) {
	auto board = space::BoardImpl::getStartingBoard();
  
	nlohmann::json config = parseConfig(argc, argv);
	auto whiteAlgo = 
		config.contains(WhiteAlgoFieldName) 
		? space::AlgoFactory::tryCreateAlgo(config[WhiteAlgoFieldName]).value()
		: space::CliAlgo::create(std::cin, std::cout);
	auto blackAlgo = 
		config.contains(BlackAlgoFieldName)
		? space::AlgoFactory::tryCreateAlgo(config[BlackAlgoFieldName]).value()
		: space::CliAlgo::create(std::cin, std::cout);
  
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