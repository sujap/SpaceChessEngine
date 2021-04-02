#pragma once

#include <chess/board.h>

#include <memory>
#include <vector>

namespace space {


	struct TestPosition {
		std::string position;
		std::map<Move, std::string> moveMap;
	};
	typedef std::shared_ptr<const TestPosition> TestPositionCPtr;


	TestPositionCPtr startingPosition();
	std::vector<TestPositionCPtr> getAllTestPositions();

	std::string printAllMoves(const std::string& position);


} // end namespace space
