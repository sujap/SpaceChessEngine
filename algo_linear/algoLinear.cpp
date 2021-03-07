#include "algoLinear.h"

#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace space {

	

	AlgoLinearDepthOne::AlgoLinearDepthOne(std::vector<double> wtsVec)
	{
		space_assert(wtsVec.size() == 5,
			"Need 5 weights for algo");
		this->weights[PieceType::Pawn] = wtsVec[0];
		this->weights[PieceType::EnPassantCapturablePawn] = wtsVec[0];
		this->weights[PieceType::Rook] = wtsVec[1];
		this->weights[PieceType::Knight] = wtsVec[2];
		this->weights[PieceType::Bishop] = wtsVec[3];
		this->weights[PieceType::Queen] = wtsVec[4];
	}

	Move AlgoLinearDepthOne::getNextMove(IBoard::Ptr board)
	{
		IBoard::MoveMap moveMap = board->getValidMoves();
		bool high = board->whoPlaysNext() == Color::White;
		std::map<Move, Score> allScores;
		for (const auto& mb : moveMap) {
			allScores[mb.first] = this->getScore(mb.second);
		}
		Move bestMove = get_best(allScores, high);

		return bestMove; 
	}

	AlgoLinearDepthOne::Score AlgoLinearDepthOne::getScore(IBoard::Ptr board)
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



// Depth Two algos


	AlgoLinearDepthTwoExt::AlgoLinearDepthTwoExt(int _breadth, std::vector<double> wtsVec)
	{
		space_assert(breadth > 0, "Breadth must be positive");
		space_assert(wtsVec.size() == 5, "Need 6 weights");
		this->breadth = _breadth;
		this->weights[PieceType::Pawn] = wtsVec[0];
		this->weights[PieceType::EnPassantCapturablePawn] = wtsVec[0];
		this->weights[PieceType::Rook] = wtsVec[1];
		this->weights[PieceType::Knight] = wtsVec[2];
		this->weights[PieceType::Bishop] = wtsVec[3];
		this->weights[PieceType::Queen] = wtsVec[4];		
	}


	AlgoLinearDepthTwoExt::Score AlgoLinearDepthTwoExt::getLinearScore(IBoard::Ptr board)
	{
		Score s = 0;
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> pMaybe = board->getPiece({ i, j });
				if (!pMaybe.has_value()) {
					continue;
				}
				Piece p = pMaybe.value();
				int sign = p.color == Color::White ? 1 : -1;
				if (p.pieceType == PieceType::Pawn || 
					p.pieceType == PieceType::EnPassantCapturablePawn) {
					s += sign * (1 + this->weights[p.pieceType] * (sign == 1 ? (i - 1) : (6 - i)));
				}
				else {
					s += sign * this->weights[p.pieceType];
				}
			}
		return s;
	}

	std::vector<AlgoLinearDepthTwoExt::ScoreTriple> AlgoLinearDepthTwoExt::getAllScores(IBoard::Ptr board)
	{
		IBoard::MoveMap moveMap = board->getValidMoves();
		std::vector<ScoreTriple> allScores;
		for (const auto& mb : moveMap) {
			allScores.push_back(std::make_tuple(mb.first,
				mb.second,
				this->getLinearScore(mb.second)));
		}

		return allScores;
	}

	AlgoLinearDepthTwoExt::ScorePair AlgoLinearDepthTwoExt::findBestLinearMove(IBoard::Ptr board)
	{
		std::vector<ScoreTriple> allScores = this->getAllScores(board);
		int direction = board->whoPlaysNext() == Color::White ? 1 : -1;

		auto cmp = [direction](const ScoreTriple& a, const ScoreTriple& b) {
			return direction * (std::get<2>(a) < std::get<2>(b)) < 0;
		};

		ScoreTriple best = *std::max_element(allScores.begin(),
			allScores.end(),
			cmp);

		return std::make_pair(std::get<0>(best), std::get<2>(best));
	}



	Move AlgoLinearDepthTwoExt::getNextMove(IBoard::Ptr board)
	{
		std::vector<ScoreTriple> allScores = this->getAllScores(board);
		int direction = board->whoPlaysNext() == Color::White ? 1 : -1;

		auto cmp = [direction](const ScoreTriple& a, const ScoreTriple& b) {
			return direction * (std::get<2>(a) < std::get<2>(b)) < 0;
		};


		std::sort(allScores.begin(), allScores.end(), cmp);
		allScores.resize(this->breadth);


		std::map<Move, Score> allScores2;
		for (const auto& v : allScores) {
			ScorePair bestDepth2 = this->findBestLinearMove(std::get<1>(v));
			allScores2[std::get<0>(v)] = std::get<1>(bestDepth2);
		}

		Move bestMove = get_best(allScores2, direction == -1);

		return bestMove;

	}

	




}
