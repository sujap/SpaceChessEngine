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

	// Castling conditions:
	// King & Rook not moved -- examined here using booleans, updated in updateBoard
	// No piece in between -- examined here directly
	// No check in 3 cells // examined [[ TODO]]

	bool BoardImpl::canCastleLeft(Color color) const
	{
		bool flag = true;
		switch (color)
		{
		case Color::White:
			flag = flag && m_WhiteKingNotMoved && m_WhiteLeftRookNotMoved;
			for (int j = 1; j <= 3; j++)
				flag = flag && (this->m_pieces[0][j].pieceType == PieceType::None);
			return flag;
		
		case Color::Black:
			flag = flag && m_BlackKingNotMoved && m_BlackLeftRookNotMoved;
			for (int j = 5; j <= 6; j++)
				flag = flag && (this->m_pieces[7][j].pieceType == PieceType::None);
			return flag;

		default:
			throw std::runtime_error("Invalid color");
		}
	}
	bool BoardImpl::canCastleRight(Color color) const
	{
		bool flag = true;
		switch (color)
		{
		case Color::White:
			flag = flag && m_WhiteKingNotMoved && m_WhiteRightRookNotMoved;
			for (int j = 5; j <= 6; j++)
				flag = flag && (this->m_pieces[0][j].pieceType == PieceType::None);
			return flag;

		case Color::Black:
			flag = flag && m_BlackKingNotMoved && m_BlackRightRookNotMoved;
			for (int j = 1; j <= 3; j++)
				flag = flag && (this->m_pieces[0][j].pieceType == PieceType::None);
			return flag;

		default:
			throw std::runtime_error("Invalid color");
		}
	}
	bool BoardImpl::isStaleMate() const
	{
		std::map<Move, Ptr> allMoves = this->getAllmoves(); //TODO

		return false;

	}
	bool BoardImpl::isCheckMate() const
	{
		throw std::runtime_error("Not yet implemented.");
	}


	std::optional<IBoard::Ptr> BoardImpl::updateBoard(Move move) const
	{
		std::shared_ptr<BoardImpl> newBoard = std::make_shared<BoardImpl>();

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				newBoard->m_pieces[i][j] = this->m_pieces[i][j];
			}
		}





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
		board->m_WhiteKingNotMoved = true;
		board->m_WhiteLeftRookNotMoved = board->m_WhiteRightRookNotMoved = true;
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
		board->m_BlackKingNotMoved = true;
		board->m_BlackLeftRookNotMoved = board->m_BlackRightRookNotMoved = true;
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
	bool BoardImpl::checkObstructions(Move m)
	{
		// pawns are examined elsewhere
		// for all other cases, we examine for target cell occupied by same color
		// for long moves (Rook, Bishop, Queen) , we examine for path obstructions,
		// no further checks for Knights

		Piece pMove = this->m_pieces[m.sourceRank][m.sourceFile];
		PieceType pType = pMove.pieceType;
		Color c = pMove.color;

		if (pType == PieceType::Pawn || pType == PieceType::EnPessantCapturablePawn) {
			return true;
		}
		
		Piece pTarget = this->m_pieces[m.destinationRank][m.destinationFile];
		PieceType pTargetType = pTarget.pieceType;
		Color ct = pTarget.color;
		
		if (pTargetType != PieceType::None && ct == c)
		{
			return false; // target cell obstruction by same color
		}

		// path obstruction calculation
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

	std::map<Move, IBoard::Ptr> BoardImpl::getAllmoves() const
	{
		return std::map<Move, Ptr>(); // TODO
	}

	std::map<Move, IBoard::Ptr> BoardImpl::getAllmoves(int rank, int file) const
	{
		Piece p = this->m_pieces[rank][file];
		PieceType t = p.pieceType;
		Color c = p.color;
		std::vector<Move> moves;

		if (t == PieceType::None || c != this->m_whoPlaysNext) {
			return std::map<Move, Ptr>();
		}

		if (t == PieceType::King) {
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
			for (int tgtRank = 0; tgtRank < 8; tgtRank++) {
				if (tgtRank != rank) {
					for (int j = -1; j <= 1; j += 2) {

						int tgtFile = file + j * (tgtRank - rank);

						if (inRange(tgtFile)) {
							moves.push_back({ rank, file, tgtRank, tgtFile });
						}
					}
				}
			}
		}

		if (t == PieceType::Knight) {
			for (int i = -1; i <= 1; i+= 2 ) {
				for (int j = -1; j <= 1; j+= 2) {
					for (int k = 1; k <= 2; k++) 
					{
						int tgtRank = rank + k * i;
						int tgtFile = file + (3 - k) * j;
						if (inRange(tgtRank) && inRange(tgtFile))
						{
							moves.push_back({ rank, file, tgtRank, tgtFile });
						}
					}
				}
			}
		}

		// TODO: Examine for obstructions


		// for pawns, we examine for obstruction and capture rules here
		if (t == PieceType::Pawn)
		{
			int direction = (c == Color::White ? 1 : -1);

			for (int j = -1; j <= 1; j++) {
				if (!inRange(file + j))
					continue;

				Piece pTgt = this->m_pieces[rank + direction][file + j];

				// checking for obstruction
				if ((j == 0 && pTgt.pieceType == PieceType::None) ||
					(j != 0 && pTgt.pieceType != PieceType::None && pTgt.color != c)) {

					// pawn promotion
					if (rank == (c == Color::White ? 6 : 1)) {
						moves.push_back({ rank, file, rank + direction, file + j,PieceType::Bishop });
						moves.push_back({ rank, file, rank + direction, file + j,PieceType::Rook });
						moves.push_back({ rank, file, rank + direction, file + j,PieceType::Queen });
						moves.push_back({ rank, file, rank + direction, file + j,PieceType::Knight });
					}
					// one step move
					else {
						moves.push_back({ rank, file, rank + direction, file + j });
						// two step from home row, only straight
						if (j == 0 && rank == (c == Color::White ? 1 : 6)) {
							Piece pStraight2 = this->m_pieces[rank + 2 * direction][file];
							if (pStraight2.pieceType == PieceType::None) {
								moves.push_back({ rank, file, rank + 2 * direction, file });
							}
						}
					}
				}
			}
		
			// en passant pawn capture

			if (rank == (direction == 1 ? 4 : 3))
			{
				for (int j = -1; j <= 1; j += 2) {
					if (inRange(file + j))
					{
						Piece pCapture = this->m_pieces[rank][file + j];
						if (pCapture.pieceType == PieceType::EnPessantCapturablePawn &&
							pCapture.color != c)
						{
							moves.push_back({ rank, file, rank + direction, file + j });
						}
					}
				}
			}
		}

		// Castling: denoted by king moving two steps

		int direction = (c == Color::White) ? 1 : -1;

		if (t == PieceType::King && canCastleLeft(c))
		{
			moves.push_back({ rank, file, rank, file - 2 * direction });
		}

		if (t == PieceType::King && canCastleRight(c))
		{
			moves.push_back({ rank, file, rank, file + 2 * direction });
		}

		// Examine for checkmate ??

		// Get corresponding resulting boards

		return std::map<Move, Ptr>();
	}

}
