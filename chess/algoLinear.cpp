#include "algoLinear.h"

#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace space {


	IAlgoLinear::Score IAlgoLinear::getLinearScore(IBoard::Ptr board)
	{
		auto signum = [](Color c) { return c == Color::White ? 1 : -1; };
		Score s = 0;

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> p = board->getPiece({ i, j });
				if (p.has_value()) {
					s += signum(p.value().color) * this->weights[p.value().pieceType];
				}
			}
		return s;

	}

	IAlgoLinear::IAlgoLinear(std::vector<double> wtsVec)
	{
		if (wtsVec.size() != 5) {
			throw std::runtime_error("Need 5 weights");
		}

		this->weights[PieceType::Pawn]						= wtsVec[0];
		this->weights[PieceType::EnPassantCapturablePawn]	= wtsVec[0];
		this->weights[PieceType::Rook]						= wtsVec[1];
		this->weights[PieceType::Knight]					= wtsVec[2];
		this->weights[PieceType::Bishop]					= wtsVec[3];
		this->weights[PieceType::Queen]						= wtsVec[4];
	}



	AlgoLinearDepthOne::Score AlgoLinearDepthOne::getScore(IBoard::Ptr board)
	{
		return this->getLinearScore(board);
	}


	AlgoLinearDepthTwoDef::Score AlgoLinearDepthTwoDef::getScore(IBoard::Ptr board)
	{
		
		return Score(); // TODO

	}

	Move AlgoLinearDepthTwoDef::getNextMove(IBoard::Ptr board)
	{
		using ScoreMap = std::pair<Move, Score>;
		using ScoreTriple = std::tuple<Move, IBoard::Ptr, Score>;
		IBoard::MoveMap  allMoves = board->getValidMoves();
		int direction = board->whoPlaysNext() == Color::White ? 1 : -1;

		std::vector<ScoreTriple> allScores;
		for (const auto& mb : allMoves) {
			allScores.push_back({ mb.first, mb.second, this->getScore(mb.second) });
		}

		auto cmp = [direction](const ScoreTriple& a, const ScoreTriple& b) {
			return direction * (std::get<2>(a) - std::get<2>(b)) < 0;
		};

		std::sort(allScores.begin(), allScores.end(), cmp);
		allScores.resize(this->breadthCount);

		std::map<Move, Score> allScores2;

		for (const auto& v: allScores) {
			allScores2[std::get<0>(v)] = std::get<2>(this->findBestMove(std::get<1>(v)));
		}

		auto bestMove = get_best(allScores2, direction == -1);

		return bestMove;
	}

}
