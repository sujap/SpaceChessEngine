#include <gtest/gtest.h>

#include "test_positions.h"

#include <chess/board_impl.h>

#include <algo_linear/algo_dumbo.h>
#include <algo_linear/algo_dumbo_impl.h>


	// TODO: implement all tests
	// testExploreStates();
	// testComputeBasicScore();
	// testConfigParsing();


TEST(AlgoDumboSuite, StateCompareTest)
{
	using namespace algo_dumbo_impl;
	State a, b;
	StateCompare c;
	auto equals = [&a, &b, &c]{ return !c(a, b) && !c(b, a); };
	auto lt = [&a, &b, &c]{ return c(a, b) && !c(b, a); };
	auto gt = [&a, &b, &c]{ return !c(a, b) && c(b, a); };
	ASSERT_TRUE(equals()) << "Default states should be equal.";

	a[0] = true;
	ASSERT_TRUE(gt()) << "True at pos 0 should be greater than false.";

	b[1] = true;
	ASSERT_TRUE(lt()) << "True at pos 1 should be greater than pos 0.";

	a[1] = b[0] = true;
	ASSERT_TRUE(equals()) << "True values should be compared correctly.";

	a[a.size() - 1] = true;
	ASSERT_TRUE(gt()) << "Last position is also compared.";
}




TEST(AlgoDumboSuite, StateBoardConversionTest)
{
	auto testPositions = space::getAllTestPositions();
	for (auto tp: testPositions)
	{
		auto board = space::BoardImpl::fromFen(tp->position);
		auto state = algo_dumbo_impl::boardToState(*board);
		auto board2 = algo_dumbo_impl::stateToBoard(state);
		ASSERT_EQ(board->canCastleLeft(space::Color::White), board2->canCastleLeft(space::Color::White));
		ASSERT_EQ(board->canCastleLeft(space::Color::Black), board2->canCastleLeft(space::Color::Black));
		ASSERT_EQ(board->canCastleRight(space::Color::White), board2->canCastleRight(space::Color::White));
		ASSERT_EQ(board->canCastleRight(space::Color::Black), board2->canCastleRight(space::Color::Black));
		ASSERT_EQ(board->isStaleMate(), board2->isStaleMate());
		ASSERT_EQ(board->isCheckMate(), board2->isCheckMate());
		ASSERT_EQ(board->whoPlaysNext(), board2->whoPlaysNext());
		for (int rank = 0; rank < 7; ++rank)
		{
			for (int file = 0; file < 7; ++file)
			{
				space::Position pos(rank, file);
				auto leftPiece = board->getPiece(pos);
				auto rightPiece = board2->getPiece(pos);
				ASSERT_EQ(leftPiece.has_value(), rightPiece.has_value());
				if (leftPiece.has_value())
				{
					ASSERT_EQ(leftPiece.value().pieceType, rightPiece.value().pieceType);
					ASSERT_EQ(leftPiece.value().color, rightPiece.value().color);
				}
			}
		}
	}
}

TEST(AlgoDumboSuite, StateOperationsTest)
{
	using namespace algo_dumbo_impl;
	using namespace space;
	
	// check empty score container
	StateScores scores;
	ASSERT_EQ(getNumUniqueStates(scores), 0);
	
	// add one state
	auto startingBoard = BoardImpl::getStartingBoard();
	auto startingState = boardToState(*startingBoard);
	addState(scores, startingState, 5);
	ASSERT_EQ(getNumUniqueStates(scores), 1);

	// add score
	const double startingScore = 50;
	setScore(scores, startingState, 0, startingScore);
	ASSERT_DOUBLE_EQ(getScore(scores, startingState, 0), startingScore);

	// check uniqueness
	auto a4e5e4 = 
		startingBoard
		->updateBoard({0, 1, 0, 3}).value()
		->updateBoard({4, 6, 4, 4}).value()
		->updateBoard({4, 1, 4, 3}).value();
	auto a4e5e4State = boardToState(*a4e5e4);
	addState(scores, a4e5e4State, 5);

	auto e4e5a4 =
		startingBoard
		->updateBoard({4, 1, 4, 3}).value()
		->updateBoard({4, 6, 4, 4}).value()
		->updateBoard({0, 1, 0, 3}).value();
	auto e4e5a4State = boardToState(*e4e5a4);
	addState(scores, e4e5a4State, 5);
	ASSERT_EQ(getNumUniqueStates(scores), 2);


	// check state set
	StateSet stateSet;
	addState(scores, stateSet, startingState, 5);
	addState(scores, stateSet, a4e5e4State, 5);
	addState(scores, stateSet, e4e5a4State, 5);
	std::set<State, StateCompare> actualSet;
	auto stateOp = [&actualSet](const StateHandle& sh)
	{
		actualSet.insert(getState(sh));
		setScore(sh, 1, 1.0);
		setScore(sh, 2, 2.0);
	};
	forEachState(stateSet, stateOp);
	std::set<State, StateCompare> expectedSet{startingState, a4e5e4State};
	ASSERT_EQ(actualSet, expectedSet);
	ASSERT_DOUBLE_EQ(getScore(scores, startingState, 1), 1.0);
	ASSERT_DOUBLE_EQ(getScore(scores, startingState, 2), 2.0);
	ASSERT_DOUBLE_EQ(getScore(scores, a4e5e4State, 1), 1.0);
	ASSERT_DOUBLE_EQ(getScore(scores, a4e5e4State, 2), 2.0);
	ASSERT_DOUBLE_EQ(getScore(scores, e4e5a4State, 1), 1.0);
	ASSERT_DOUBLE_EQ(getScore(scores, e4e5a4State, 2), 2.0);

}


TEST(AlgoDumboSuite, StateHandleCompareTest)
{
	using namespace algo_dumbo_impl;
	auto startingBoard = space::BoardImpl::getStartingBoard();
	auto startingState = boardToState(*startingBoard);
	auto a4 = startingBoard->updateBoard({0, 1, 0, 3});
	auto a4State = boardToState(**a4);
	auto e4 = startingBoard->updateBoard({4, 1, 4, 3});
	auto e4State = boardToState(**e4);
	StateScores scores;
	StateSet stateSet;
	addState(scores, stateSet, startingState, 3);
	addState(scores, stateSet, a4State, 3);
	addState(scores, stateSet, e4State, 3);
	forEachState(
			stateSet,
			[&stateSet](const StateHandle& sh1)
			{
			    auto s1 = getState(sh1);
				forEachState(
						stateSet,
						[&s1, &sh1](const StateHandle& sh2)
						{
						    auto s2 = getState(sh2);
							ASSERT_EQ(StateCompare()(s1, s2), StateHandleCompare()(sh1, sh2));
						});
			});

}



TEST(AlgoDumboSuite, ScoreFactorForColorTest)
{
	auto b = algo_dumbo_impl::getScoreFactorForColor(space::Color::Black);
	auto w = algo_dumbo_impl::getScoreFactorForColor(space::Color::White);
	ASSERT_DOUBLE_EQ(b * w, -1.0);
	ASSERT_DOUBLE_EQ(std::abs(b), 1.0);
	ASSERT_DOUBLE_EQ(std::abs(w), 1.0);
}


TEST(AlgoDumboSuite, ComparatorForColorTest)
{
	const double g = 5, l = 3;
	const double gb = g * algo_dumbo_impl::getScoreFactorForColor(space::Color::Black);
	const double gw = g * algo_dumbo_impl::getScoreFactorForColor(space::Color::White);
	const double lb = l * algo_dumbo_impl::getScoreFactorForColor(space::Color::Black);
	const double lw = l * algo_dumbo_impl::getScoreFactorForColor(space::Color::White);

	const auto cb = algo_dumbo_impl::getComparatorForColor(space::Color::Black);
	const auto cw = algo_dumbo_impl::getComparatorForColor(space::Color::White);
	
	ASSERT_TRUE(cb(lb, gb));
	ASSERT_TRUE(cw(lw, gw));
}

