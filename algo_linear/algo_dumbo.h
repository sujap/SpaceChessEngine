#pragma once


#include <chess/algo.h>
#include <nlohmann/json.hpp>


namespace space {

	struct AlgoDumboConfig {
		double maxDepth;
		double maxNumStates;
		double maxScore;
		double pawnScore;
		double rookScore;
		double knightScore;
		double bishopScore;
		double queenScore;
		double validMoveScore;
		AlgoDumboConfig();
		AlgoDumboConfig(const nlohmann::json& config);
	};

	class AlgoDumbo: public IAlgo {
		public:
			using Ptr = std::shared_ptr<AlgoDumbo>;
			Move getNextMove(IBoard::Ptr board) override;

			AlgoDumbo();
			AlgoDumbo(const nlohmann::json& config);

		private:
			AlgoDumboConfig m_config;

	};


} // end namespace space

