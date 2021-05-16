#include "algoGeneric.h"

#include <algorithm>


namespace space {


	int Node::objectCount;


	Node::Node(IBoard::Ptr v_board, double s, unsigned int v_depth) : board(v_board), score(s), depth(v_depth)
	{
		// this->fen = Fen::fromBoard(v_board, 0, 0).fen;
		this->direction = colorToSign(v_board->whoPlaysNext());
		++this->objectCount;
	}

	Move Node::bestMove()
	{
		space_assert(this->children.size() > 0,
						"No moves available");

		int direction = this->direction;
		using NodePair = std::pair<Move, Node::Ptr>;
		auto cmp = [direction](const NodePair& a, const NodePair& b) {
			return direction * (a.second->score - b.second->score) < 0;
		}; // a better than b

		auto best = *std::max_element(this->children.begin(), this->children.end(), cmp);

		return best.first;
	}

	int Node::familySize() const
	{
		int count = 1;
		for (const auto& child : this->children)
			count += child.second->familySize();
		return count;
	}

	// ---------   Pruning & Folding   --------



	Pruning_Cutoff::Score Pruning_Cutoff::getScoreDiff(int depth) const
	{
		return this->scoreDiffVec.size() > depth ? this->scoreDiffVec[depth] : this->scoreDiffVec.back();
	}

	int Pruning_Cutoff::getMaxMoves(int depth) const
	{
		return this->maxMovesVec.size() > depth ? this->maxMovesVec[depth] : this->maxMovesVec.back();
	}

	void Pruning_Cutoff::pruning(Node::Ptr node)
	{
		// create a ScoreVec 
		// call helper pruning function
		// erase children accordingly

		ScoreVec sv;
		for (const auto& childPair : node->children) {
			sv.push_back(std::make_pair(childPair.first, childPair.second->score));
		}

		int nMoves = this->getMaxMoves(node->depth);
		Score sDelta = this->getScoreDiff(node->depth);

		std::vector<Move> moves = this->pruning(sv,nMoves, sDelta,  node->direction);
		for (const Move& m : moves) {
			node->children.erase(m);
		}
	}


	// return a vector of moves to be removed
	std::vector<Move> Pruning_Cutoff::pruning(ScoreVec sv, int maxMoves, Score scoreDiff, int  direction)
	{
		int n = sv.size();
		if (n == 0) {
			return {};
		}
		int nMin = std::min(n, maxMoves);

		auto cmp = [direction](const ScorePair& a, const ScorePair& b) {
			return direction *  (a.second - b.second) > 0;
		}; // a better than b
		
		std::sort(sv.begin(), sv.end(), cmp); // better moves first

		Score scoreCutoff = sv[0].second - scoreDiff * direction;

		auto cmp2 = [scoreCutoff, direction](const ScorePair& sp) {
			return direction * (sp.second - scoreCutoff) > 0;  
		};

		int dCount = std::count_if(sv.begin(), sv.end(), cmp2);

		int nFinal = this->bothCriteria ? std::min(dCount, nMin) : std::max(dCount, nMin);

		std::vector<Move> moves;

		for (int i = nFinal; i < n; i++)
			moves.push_back(sv[i].first);

		return moves;
	}

	

	void Fold_MinMax::fold(Node::Ptr node)
	{
		if (node->children.size() > 0) {
			Move bestMove = node->bestMove();
			node->score = node->children[bestMove]->score;
		}
	}


	//-------------------------------------------------------------------------
	//Algo Generic


	AlgoGeneric::Score AlgoGeneric::getLinearScore(IBoard::Ptr board)
	{
		Score whiteScore = 0,blackScore = 0;
		for (const auto v : this->wts) {
			whiteScore += v.first->getValue(board, Color::White) * v.second;
			blackScore += v.first->getValue(board, Color::Black) * v.second;
		}
		return whiteScore - blackScore;
	}

	void AlgoGeneric::expand()
	{
		this->expand(this->root);
	}

	void AlgoGeneric::expand(Node::Ptr node)
	{
		if (node->children.size() == 0)
			this->expandLeafNode(node);
		else {
			for (auto& mn : node->children) {
				this->expand(mn.second);
			}
		}
	}

	void AlgoGeneric::expandLeafNode(Node::Ptr node)
	{		
		space_assert(node->board.has_value(), "A leaf node must have board object");
		IBoard::MoveMap allMoves = node->board.value()->getValidMoves();
		for (const auto& mb : allMoves) 
		{
			Score score;
			if (mb.second->isCheckMate()) {
				int direction = colorToSign(mb.second->whoPlaysNext());
				score = AlgoGeneric::scoreMax * direction;
			}
			else if (mb.second->isStaleMate())
				score = 0;
			else
				score = 42.0; // this->getLinearScore(mb.second); // 18 * urand();
			Node childNode = Node(mb.second, score, node->depth + 1);
			node->children[mb.first] = std::make_shared<Node>(childNode);
		}
		node->board = std::nullopt;
	}


	// Postorder traversal of gameTree
	// if leaf node -> do nothing
	// else -> call refresh on each child
	//         apply pruning on children
	//         fold to get (updated) score for current node
	//         delete board if present 
	// Q: stack overflow ? seems max_memory <= height of tree + max Children coount


	void AlgoGeneric::refresh()
	{
		refreshNode(this->root);
	}

	void AlgoGeneric::refreshNode(Node::Ptr node)
	{
		if (node->children.size() == 0) { // Nothing to do for leaf nodes
			return;
		}
		for (auto child : node->children) {
			this->refreshNode(child.second);
		}		
		this->prune->pruning(node);
		this->rec->fold(node);
	}




	//-------------------------------------------------------------------------
	// Algo 442



	Algo442::Algo442()
	{
		//feature with wts
		this->wts[std::make_shared<Feature_Piece>(Feature_Piece(PieceType::Pawn))] = 1;
		this->wts[std::make_shared<Feature_Piece>(Feature_Piece(PieceType::Knight))] = 3;
		this->wts[std::make_shared<Feature_Piece>(Feature_Piece(PieceType::Bishop))] = 3.2;
		this->wts[std::make_shared<Feature_Piece>(Feature_Piece(PieceType::Rook))] = 5;
		this->wts[std::make_shared<Feature_Piece>(Feature_Piece(PieceType::Queen))] = 9;
		
		for(int file = 0; file< 8; file++)
			this->wts[std::make_shared<Feature_PawnRank>(Feature_PawnRank(file))] = 0.2;

		//this->wts[std::make_shared<Feature_PassedPawn>(Feature_PassedPawn())] = 0.2;
		
		
		// this->wts[std::make_shared<Feature_MoveCount>(Feature_MoveCount())] = 0.1;
		
		

		// pruning
		this->prune = std::make_shared<Pruning_Cutoff>(Pruning_Cutoff(8, 10, true));

		// fold
		this->rec = std::make_shared<Fold_MinMax>(Fold_MinMax());

	}



	Move Algo442::getNextMove(IBoard::Ptr board)
	{
		root = std::make_shared<Node>(Node(board, 
											this->getLinearScore(board),
											0));

	/*	for (int k = 0; k < 8; k++) {
			expand();
			refresh();
		}  */

		int gameTreeSize;

		expand(); 
		gameTreeSize = root->familySize();
		refresh();
		gameTreeSize = root->familySize();
		
		expand();
		gameTreeSize = root->familySize();
		refresh();
		gameTreeSize = root->familySize();
		
		expand();
		gameTreeSize = root->familySize();
		refresh();
		gameTreeSize = root->familySize();

		expand();
		gameTreeSize = root->familySize();
		refresh();
		gameTreeSize = root->familySize();


		return this->root->bestMove();
	}

}




