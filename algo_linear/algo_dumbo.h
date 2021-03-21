#pragma once


#include <chess/algo.h>
#include <nlohmann/json.hpp>


namespace space {

	struct AlgoDumboConfig {
		double maxScore;
		double castlingScore;
		double pawnScore;
		double rookScore;
		double knightScore;
		double bishopScore;
		double queenScore;
	};

	class AlgoDumbo: public IAlgo {
		public:
			using Ptr = std::shared_ptr<AlgoDumbo>;
			Move getNextMove(IBoard::Ptr board) override;

			AlgoDumbo();
			AlgoDumbo(const nlohmann::json& config);

	};


} // end namespace space

