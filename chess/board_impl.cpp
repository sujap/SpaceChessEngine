#include "board_impl.h"

#include <stdexcept>

namespace space {

	Color BoardImpl::whoPlaysNext() const
	{
		return m_whoPlaysNext;
	}
	std::optional<Piece> BoardImpl::getPiece(Position position) const
	{
		if (m_pieces[position.file][position.rank].pieceType == PieceType::None)
			return std::optional<Piece>();
		else
			return m_pieces[position.file][position.rank];
	}
	bool BoardImpl::canCastleLeft(Color color) const
	{
		switch (color)
		{
		case Color::White:
			return m_canWhiteCastleLeft;
		case Color::Black:
			return m_canBlackCastleLeft;
		default:
			throw std::runtime_error("Invalid color");
		}
	}
	bool BoardImpl::canCastleRight(Color color) const
	{
		switch (color)
		{
		case Color::White:
			return m_canWhiteCastleRight;
		case Color::Black:
			return m_canBlackCastleRight;
		default:
			throw std::runtime_error("Invalid color");
		}
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

	IBoard::Ptr BoardImpl::getStartingBoard()
	{
		std::shared_ptr<BoardImpl> board = std::make_shared<BoardImpl>();
		
		// white pieces
		board->m_pieces[0][0] = { PieceType::Rook, Color::White };
		board->m_pieces[1][0] = { PieceType::Knight, Color::White };
		board->m_pieces[2][0] = { PieceType::Bishop, Color::White };
		board->m_pieces[3][0] = { PieceType::Queen, Color::White };
		board->m_pieces[4][0] = { PieceType::King, Color::White };
		board->m_pieces[5][0] = { PieceType::Bishop, Color::White };
		board->m_pieces[6][0] = { PieceType::Knight, Color::White };
		board->m_pieces[7][0] = { PieceType::Rook, Color::White };
		board->m_canWhiteCastleLeft = board->m_canWhiteCastleRight = true;
		for (std::size_t i = 0; i < 8; ++i)
			board->m_pieces[i][1] = { PieceType::Pawn, Color::White };

		// black pieces
		board->m_pieces[0][7] = { PieceType::Rook, Color::Black };
		board->m_pieces[1][7] = { PieceType::Knight, Color::Black };
		board->m_pieces[2][7] = { PieceType::Bishop, Color::Black };
		board->m_pieces[3][7] = { PieceType::Queen, Color::Black };
		board->m_pieces[4][7] = { PieceType::King, Color::Black };
		board->m_pieces[5][7] = { PieceType::Bishop, Color::Black };
		board->m_pieces[6][7] = { PieceType::Knight, Color::Black };
		board->m_pieces[7][7] = { PieceType::Rook, Color::Black };
		board->m_canWhiteCastleLeft = board->m_canWhiteCastleRight = true;
		for (std::size_t i = 0; i < 8; ++i)
			board->m_pieces[i][6] = { PieceType::Pawn, Color::Black };

		// empty positions
		for (std::size_t file = 0; file < 7; ++file)
			for (std::size_t rank = 2; rank < 6; ++rank)
				board->m_pieces[file][rank] = { PieceType::None, Color::White };

		// white make the first move
		board->m_whoPlaysNext = Color::White;

		return board;
	}
}
