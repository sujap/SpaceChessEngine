#pragma once

#include <chess/board.h>
#include <chess/algo.h>
#include <chess/fen.h>
#include <common/base.h>

#include "feature.h"



namespace space {

	struct Interval {
		double left, right;
	};

	class NodeInterval {
		//TODO
	public:
		using Ptr = std::shared_ptr<NodeInterval>;
		using NodeMap = std::map<Move, Ptr>;
		using Score = double;

		// std::string fen;  // for analysis only
		std::optional<IBoard::Ptr> board;   // removed to save space when possible
		int direction = 1; // 1 for White, -1 for Black   //TODO const
		const unsigned int depth = 0; // starts at zero        

		NodeMap children;
		Interval iscore;

		NodeInterval()
		{
			++objectCount;
		} // just for initialization of algo
		NodeInterval(IBoard::Ptr v_board, unsigned int v_depth);

		int familySize() const; // for analysis only

		static int objectCount;
	};


	// abstract class, getNextMove not implemented
	class AlgoInterval : public IAlgo {
	public:
		virtual Interval getIntervalScore()=0;
		void prune(NodeInterval::Ptr);
		void fold(NodeInterval::Ptr);
		void expand();
		void expand(NodeInterval::Ptr node); // expand all leaf nodes to one more level
		void expandLeafNode(NodeInterval::Ptr node); // expand leaf node one level to all possible subsequent moves
		void refresh(); // pruning & folding alternately from bottom-up
		void refreshNode(NodeInterval::Ptr node); // for a single node


	protected:
		NodeInterval::Ptr root;


	};






}



