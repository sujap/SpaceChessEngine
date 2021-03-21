#include "algo_dumbo.h"
#include <chess/board.h>
#include <stdexcept>


#define NYI throw std::runtime_error("Not yet implemented")

namespace {

	using State = std::vector<bool>;
	space::IBoard::Ptr stateToBoard(const State& state) { NYI; }
	State boardToState(const space::IBoard& board) { NYI; }

	using StateScores = std::map<State, std::vector<double> >;
	using StateScoresCIt = StateScores::const_iterator;
	int getNumUniqueStates(const StateScores& stateScores) { NYI; }
	void addState(StateScores& stateScores, const State& state) { NYI; }
	void setScore(StateScores& stateScores, const State& state, int depth, double score) { NYI; }
	double getScore(const StateScores& stateScores, const State& state, int depth) { NYI; }
	State getStateWithHighestScore(const StateScores & stateScores) { NYI; }
	StateScoresCIt getBeginStateScoresCIt(const StateScores& stateScores) { return stateScores.cbegin(); }
	StateScoresCIt getEndStateScoresCIt(const StateScores& stateScores) { return stateScores.cend(); }
	

	void exploreStates(
			StateScores& stateScores,
			int maxStates,
			int maxDepth,
			space::Color whoPlaysNext);

	double computeBasicScore(
			const State& state,
			const space::AlgoDumboConfig& config);

	double computeBasicScore(
			const space::IBoard& board,
			const space::AlgoDumboConfig& config,
			space::Color color);


} //end anonymous namespace





namespace space {

	Move AlgoDumbo::getNextMove(IBoard::Ptr board) {


		if (board->isCheckMate())
			throw std::runtime_error("Cannot find next move for a board on check mate.");
		if (board->isStaleMate())
			throw std::runtime_error("Cannot find next move for a board on stale mate.");

		const int MAX_STATES = 1000 * 1000;
		const int MAX_DEPTH = 5;

		std::vector<std::pair<Move, State> > movesAndStates;
		StateScores stateScores;
		
		for (const auto& moveXboard: board->getValidMoves()) {
			State state = boardToState(*moveXboard.second);
			movesAndStates.push_back(std::make_pair(moveXboard.first, state));
			addState(stateScores, state);
		}

		exploreStates(
				stateScores, 
				MAX_STATES - getNumUniqueStates(stateScores),
				MAX_DEPTH - 1,
				board->whoPlaysNext());


		auto stateWithHighestScore = getStateWithHighestScore(stateScores);
		for (const auto & moveXstate: movesAndStates)
			if (moveXstate.second == stateWithHighestScore)
				return moveXstate.first;
		
		throw std::runtime_error("Something is wrong: apparently the best move isn't valid!!!");

	}

	AlgoDumbo::AlgoDumbo() {
		throw std::runtime_error("AlgoDumbo::AlgoDumbo not yet implemented");
	}

	AlgoDumbo::AlgoDumbo(const nlohmann::json& config) {
		throw std::runtime_error("AlgoDumbo::AlgoDumbo(json) not yet implemented");
	}


} // end namespace space





namespace {

	void exploreStates(
			StateScores & stateScores,
			int maxStates,
			int maxDepth,
			space::Color whoPlaysNext)
	{
		NYI;
	}

	double computeBasicScore(
			const State& state,
			const space::AlgoDumboConfig& config)
	{
		auto board = stateToBoard(state);

		// stalemate is a draw
		if (board->isStaleMate())
			return 0;

		// checkmate is maxScore
		if (board->isCheckMate())
			if (board->whoPlaysNext() == space::Color::White) // black wins
				return config.maxScore * -1.0;
			else // white wins
				return config.maxScore;

		double blackScore = computeBasicScore(*board, config, space::Color::Black);
		double whiteScore = computeBasicScore(*board, config, space::Color::White);
		return whiteScore - blackScore;
	}

	double computeBasicScore(
			const space::IBoard& board,
			const space::AlgoDumboConfig& config,
			space::Color color)
	{
		double totalScore = 0;
		
		// castling score
		if (board.canCastleLeft(color))
			totalScore += config.castlingScore;
		if (board.canCastleRight(color))
			totalScore += config.castlingScore;

		// score for pieces
		for (int rank = 0; rank < 8; ++rank)
		{
			for (int file = 0; file < 8; ++file)
			{
				space::Position pos(rank, file);
				auto optPiece = board.getPiece(pos);
				if (optPiece && optPiece->color == color)
				{
					switch (optPiece->pieceType)
					{
						case space::PieceType::Pawn:
						case space::PieceType::EnPassantCapturablePawn:
							totalScore += config.pawnScore;
							break;
						case space::PieceType::Rook:
							totalScore += config.rookScore;
							break;
						case space::PieceType::Knight:
							totalScore += config.knightScore;
							break;
						case space::PieceType::Bishop:
							totalScore += config.bishopScore;
							break;
						case space::PieceType::Queen:
							totalScore += config.queenScore;
							break;
						default:
							break;
					}
				}
			}
		}
		return totalScore;
	}

} // end anonymous namespace

































