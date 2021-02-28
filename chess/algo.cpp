#include "algo.h"
#include <algorithm>




namespace space {

	IBoard::Ptr IGame::makeNextMove(Move move)
	{
		return IBoard::Ptr();  // TODO
	}

	Move IGame::getNextMove()
	{
		return Move();  //  // TODO
	}

	std::tuple<Move, IBoard::Ptr, IScoreBasedAlgo::Score> IScoreBasedAlgo::findBestMove(IBoard::MoveMap moveMap)
	{
		std::map<Move, IScoreBasedAlgo::Score> allScores;
		bool high = true;
		for (const auto& mb : moveMap) {
			allScores[mb.first] = this->getScore(mb.second);
			high = mb.second->whoPlaysNext() == Color::White ? false : true;
		}

		Move bestMove = get_best(allScores, high);

		return std::make_tuple(bestMove, moveMap[bestMove], allScores[bestMove]) ;
	}

	std::tuple<Move, IBoard::Ptr, IScoreBasedAlgo::Score> IScoreBasedAlgo::findBestMove(IBoard::Ptr board)
	{
		IBoard::MoveMap moveMap = board->getValidMoves();
		auto bestMove = this->findBestMove(moveMap);
		return bestMove;
	}

	Move IScoreBasedAlgo::getNextMove(IBoard::Ptr board)
	{
		auto bestMove =  this->findBestMove(board);
		return std::get<0>(bestMove);
	}


}


