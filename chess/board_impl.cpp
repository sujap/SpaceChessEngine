#include "board_impl.h"

#include <stdexcept>

namespace space {

	Color BoardImpl::getNextMove() const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	std::optional<Piece> BoardImpl::getPiece(Position position) const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	bool BoardImpl::canCastleLeft(Color color) const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	bool BoardImpl::canCasleRight(Color color) const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	bool BoardImpl::isStaleMate() const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	bool BoardImpl::isCheckMate() const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	std::optional<IBoard::Ptr> BoardImpl::move(Move move) const
	{
		throw std::runtime_error("Not yet implemented.");
	}
	std::map<Move, IBoard::Ptr> BoardImpl::getPossibleMoves() const
	{
		throw std::runtime_error("Not yet implemented.");
	}
}