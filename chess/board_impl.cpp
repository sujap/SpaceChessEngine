#include "board_impl.h"

#include <stdexcept>
#include <vector>

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
	std::optional<IBoard::Ptr> BoardImpl::updateBoard(Move move) const
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
		for (uint i = 0; i < 8; ++i)
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
		for (uint i = 0; i < 8; ++i)
			board->m_pieces[i][6] = { PieceType::Pawn, Color::Black };

		// empty positions
		for (uint file = 0; file < 7; ++file)
			for (std::size_t rank = 2; rank < 6; ++rank)
				board->m_pieces[file][rank] = { PieceType::None, Color::White };

		// white make the first move
		board->m_whoPlaysNext = Color::White;

		return board;
	}

	// In this we check for obstructions
	bool BoardImpl::isLegalMove(Move m)
	{
		// pawns are ignored here
		// for all other cases, we check for target cell occupied by same color
		// for long moves (Rook, Bishop, Queen) , we check for path obstructions,
		// no further checks for Knights

		std::shared_ptr<BoardImpl> board = std::make_shared<BoardImpl>();


		Piece pMove = board->m_pieces[m.sourceRank][m.sourceFile];
		PieceType pType = pMove.pieceType;
		Color c = pMove.color;

		if (pType == PieceType::Pawn) {
			return true;
		}
		
		Piece pTarget = board->m_pieces[m.destinationRank][m.destinationFile];
		PieceType pTargetType = pTarget.pieceType;
		Color ct = pTarget.color;
		
		if (pTargetType != PieceType::None && ct == c)
		{
			return false; // target cell obstruction by same color
		}

		// path obstruction check
		if (pType == PieceType::Rook || pType == PieceType::Bishop || pType == PieceType::Queen) 
		{
			int deltax = m.destinationRank - m.sourceRank;
			int deltay = m.destinationFile - m.sourceFile;
			int delta = std::max(abs(deltax), abs(deltay));
			int sgnx = deltax > 0 ? 1 : (deltax < 0 ? -1 : 0);
			int sgny = deltay > 0 ? 1 : (deltay < 0 ? -1 : 0);

			for (int j = 1; j < delta; j++)
			{
				int rank = m.sourceRank + j * sgnx;
				int file = m.sourceFile + j * sgny;
				if (m_pieces[rank][file].pieceType != PieceType::None)
				{
					return false;
				}
			}
		}
		return true;

	}

	std::map<Move, IBoard::Ptr> BoardImpl::getAllmoves(int rank, int file)
	{
		std::shared_ptr<BoardImpl> board = std::make_shared<BoardImpl>();

		Piece p = board->m_pieces[rank][file];
		PieceType t = p.pieceType;
		Color c = p.color;
		std::vector<Move> moves;

		if (t == PieceType::None || c != board->m_whoPlaysNext) {
			return std::map<Move, Ptr>();
		}
		else if (t == PieceType::King) {
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (((i != 0) || (j != 0)) && 
						 inRange(i + rank) && inRange(j + file) ) 
					{
						moves.push_back({ rank, file, i + rank, j + file });
					}
				}
			}
		}

		else if (t == PieceType::Rook || t == PieceType::Queen) {
			for (int j = 0; j < 8; j++) {
				if (j != rank)
					moves.push_back({ rank, file, j, file });
				if (j != file)
					moves.push_back({ rank, file, rank, j });
			}
		}

		if (t == PieceType::Bishop || t == PieceType::Queen) {
			for (int j = 0; j < 8; j++) {
				int tgtRank = j;
				int tgtFile = file - rank + j;

				if ( tgtRank != rank && inRange(tgtFile) )
				{
					moves.push_back({ rank, file, tgtRank, tgtFile });
				}

				tgtRank = rank - file + j;
				tgtFile = j;

				if (tgtFile != file && inRange(tgtRank) )
				{
					moves.push_back({ rank, file, tgtRank, tgtFile });
				}
			}
		}

		if (t == PieceType::Knight) {
			for (int i = 0; i <= 1; i++) {
				for (int j = 0; j <= 1; j++) {
					for (int k = 0; k <= 1; k++) 
					{
						int tgtRank = rank + ( k == 0 ? 2 * i -1 : 4 * i - 2);
						int tgtFile = file + ( k == 1 ? 2 * j - 1: 4 * j - 2);
						if (inRange(tgtRank) && inRange(tgtFile))
						{
							moves.push_back({ rank, file, tgtRank, tgtFile });
						}
					}					
				}
			}
		}

		// TODO: Check for obstructions


		// for pawns, we check for obstruction and capture rules here
		if (t == PieceType::Pawn) 
		{
			int direction = c == Color::White ? 1 : -1;

			Piece pStraight = board->m_pieces[rank + direction][file];
			if (pStraight.pieceType == PieceType::None)
			{
				moves.push_back({ rank, file, rank + direction, file });
			}

			for (int j = -1; j <= 1; j += 2)
			{
				if (inRange(file + j))
				{
					Piece pDiag = board->m_pieces[rank + direction][file + j];
					if (pDiag.pieceType != PieceType::None && pDiag.color != c)
					{
						moves.push_back({ rank, file, rank + direction, file + j });
					}
				}
			}

			// en passant pawn capture

			if (rank == (direction == 1 ? 4 : 3))
			{
				for (int j = -1; j <= 1; j += 2) {
					if (inRange(file + j))
					{
						Piece pCapture = board->m_pieces[rank][file + j];
						if (pCapture.pieceType == PieceType::EnPessantCapturablePawn &&
							pCapture.color != c)
						{
							moves.push_back({ rank, file, rank + direction, file + j });
						}
					}
				}
			}

			// pawn promotion is ignored here, to be covered while refreshing board
			// Q: pawn promotion always Queen or many possible moves ?
			// Attn: struct Move doesnt allow for promotion type
		}

		// TODO: castling
		// Castling handled here, based on booleans already present
		// checks including obstructions to be done in board update 
		// denoted by king moving two steps

		int direction = (c == Color::White) ? 1 : -1;

		if (t == PieceType::King && canCastleLeft(c))
		{
			moves.push_back({ rank, file, rank, file - 2 * direction });
		}

		if (t == PieceType::King && canCastleRight(c))
		{
			moves.push_back({ rank, file, rank, file + 2 * direction });
		}

		// Check for checkmate ??

		// Get corresponding resulting boards

		return std::map<Move, Ptr>(); // TODO
	}







}
