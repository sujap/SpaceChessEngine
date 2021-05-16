#pragma once

#include <chess/board.h>
#include <chess/algo.h>
#include <chess/fen.h>
#include <common/base.h>

#include "feature.h"



namespace space {

	class Node {
	public:
		using Ptr = std::shared_ptr<Node>;
		using NodeMap = std::map<Move, Ptr>;
		using Score = double;

		// std::string fen;  // for analysis only
		std::optional<IBoard::Ptr> board;   // removed to save space when possible
		int direction; // 1 for White, -1 for Black   //TODO const
		const unsigned int depth=0; // starts at zero        

		NodeMap children;
		Score score;

		Node() 
		{
			++objectCount;
		} // just for initialization of algo
		Node& operator=(const Node&) = default;
		Node(const Node&) = default;
		Node(IBoard::Ptr v_board, double s, unsigned int v_depth);

		Move bestMove(); // move with best score among children
		int familySize() const; // for analysis only

		static int objectCount;

	};

	// abstract classes

	class Pruning {
	public:
		using Ptr = std::shared_ptr<Pruning>;
		virtual void pruning(Node::Ptr) = 0;
	};


	class Fold {
	public:
		using Ptr = std::shared_ptr<Fold>;
		using Score = double;
		virtual void fold(Node::Ptr) = 0;
	};


	// concrete classes

	class Pruning_Cutoff : public Pruning {
	public:
		using Score = double;
		using ScorePair = std::pair<Move, Score>;
		using ScoreVec = std::vector<ScorePair>;
		void pruning(Node::Ptr) override;

		Pruning_Cutoff(int v_maxMoves, Score v_scoreDiff, bool v_both = true) :
			maxMovesVec(1, v_maxMoves), scoreDiffVec(1, v_scoreDiff), bothCriteria(v_both) {};

		Pruning_Cutoff(std::vector<int> v_maxMovesVec, std::vector<Score> v_scoreDiffVec, bool v_both = true) :
			maxMovesVec(v_maxMovesVec), scoreDiffVec(v_scoreDiffVec), bothCriteria(v_both) {}

		bool bothCriteria;
	private:
		std::vector<Move> pruning(ScoreVec sv, int maxMoves, Score scoreDiff, int  direction);
		Score getScoreDiff(int depth) const;
		int getMaxMoves(int depth) const;

		std::vector<int> maxMovesVec;
		std::vector<Score> scoreDiffVec;

	};

	class Fold_MinMax : public Fold {
		void fold(Node::Ptr) override;
	};



	// algorithm

	// abstract class since getNextMove is not overridden
	class AlgoGeneric : public IAlgo {
	public:
		using Ptr = std::shared_ptr<AlgoGeneric>;
		using Score = double;
		using FeatureMap = std::map<Feature::Ptr, double>;

		AlgoGeneric() {}
		AlgoGeneric(FeatureMap v_wts, Pruning::Ptr v_pr, Fold::Ptr v_rc) :
		   wts(v_wts), prune(v_pr), rec(v_rc){}

		static constexpr Score scoreMax = 1e8; 
		
		Score getLinearScore(IBoard::Ptr board);


	protected:
		FeatureMap wts;
		Pruning::Ptr prune;
		Fold::Ptr rec;


		// storage objects
		Node::Ptr root;


		// helper functions
		void expand();
		void expand(Node::Ptr node); // expand all leaf nodes to one more level
		void expandLeafNode(Node::Ptr node); // expand leaf node one level to all possible subsequent moves
		void refresh(); // pruning & folding alternately from bottom-up
		void refreshNode(Node::Ptr node); // for a single node

	};

	class Algo442 final : public AlgoGeneric {
	public: 
		Move getNextMove(IBoard::Ptr board) override;
		Algo442();

	};


}