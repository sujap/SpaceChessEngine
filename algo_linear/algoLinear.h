#pragma once


#include "chess/board.h"
#include "chess/algo.h"

#include "common/base.h"

#include <string>
#include <vector>

namespace space {
	int x;

	
	class AlgoLinearDepthOne: public IAlgo {
	public:
		using Score = double;
		AlgoLinearDepthOne(std::vector<double> wtsVec);
		Move getNextMove(IBoard::Ptr board) override;

	protected:
		std::map<PieceType, double> weights;
		Score getScore(IBoard::Ptr board);

   };

	class AlgoLinearDepthTwoExt : public IAlgo {
	public:
		using Score = double;
		using ScorePair = std::pair<Move, Score>;
		using ScoreTriple = std::tuple<Move, IBoard::Ptr, Score>;

		AlgoLinearDepthTwoExt(int breadth, std::vector<double> wtsVec);
		Move getNextMove(IBoard::Ptr board) override;

	protected:
		int breadth;
		std::map<PieceType, double> weights;
		Score getLinearScore(IBoard::Ptr board);
		std::vector<ScoreTriple> getAllScores(IBoard::Ptr board);
		ScorePair findBestLinearMove(IBoard::Ptr board);



	};


	
}

