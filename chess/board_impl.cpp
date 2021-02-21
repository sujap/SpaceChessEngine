#include "board_impl.h"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

	space::PieceType toPieceType(char c)
	{
		using namespace space;
		// Pawn, EnPessantCapturablePawn, Rook, Knight, Bishop, Queen, King, None
		switch (c)
		{
		case 'p':
		case 'P':
			return PieceType::Pawn;
		case 'r':
		case 'R':
			return PieceType::Rook;
		case 'n':
		case 'N':
			return PieceType::Knight;
		case 'b':
		case 'B':
			return PieceType::Bishop;
		case 'q':
		case 'Q':
			return PieceType::Queen;
		case 'k':
		case 'K':
			return PieceType::King;
		default:
			throw std::runtime_error(std::string("Unrecognizable piece type '") + c + "'");
		}
	}

} // end anonymous namespace



namespace space {

	Color BoardImpl::whoPlaysNext() const
	{
		return m_whoPlaysNext;
	}

	Color BoardImpl::getColor(bool current = true) const{
		return (current == (this->m_whoPlaysNext == Color::White) ? Color::White : Color::Black);
	}

	std::optional<Piece> BoardImpl::getPiece(Position position) const
	{
		if (m_pieces[position.rank][position.file].pieceType == PieceType::None)
			return std::optional<Piece>();
		else
			return m_pieces[position.rank][position.file];
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
			flag = flag && this->m_canWhiteCastleLeft;
			for (int j = 1; j <= 3; j++)
				flag = flag && (this->m_pieces[0][j].pieceType == PieceType::None);
			return flag;
		
		case Color::Black:
			flag = flag && this->m_canBlackCastleLeft;
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
			flag = flag && this->m_canWhiteCastleRight;
			for (int j = 5; j <= 6; j++)
				flag = flag && (this->m_pieces[0][j].pieceType == PieceType::None);
			return flag;

		case Color::Black:
			flag = flag && this->m_canBlackCastleRight;
			for (int j = 1; j <= 3; j++)
				flag = flag && (this->m_pieces[0][j].pieceType == PieceType::None);
			return flag;

		default:
			throw std::runtime_error("Invalid color");
		}
	}
	


	bool BoardImpl::isCheckMate(int rank, int file,bool current = true) const
	{
		Color c = this->getColor(current);
		if (rank == -1 || file == -1) { // locate king
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					Piece p = this->m_pieces[i][j];
					if(p.color != c && p.pieceType == PieceType::King){
						rank = i;
						file = j;
					}
				}
			}
		}

		std::vector<Move> allMoves = this->getAllmovesWithoutObstructions(current);
		for (Move m : allMoves) {
			if (m.destinationRank == rank && m.destinationFile == file)
				return true;
		}
		return false;
	}




	bool BoardImpl::isStaleMate() const
	{
		std::map<Move, IBoard::Ptr> allMoves = this->getPossibleMoves();

		return (allMoves.size() == 0);

	}

	bool BoardImpl::isCheckMate() const
	{
		return this->isCheckMate(-1, -1, true);
	}


	std::optional<IBoard::Ptr> BoardImpl::updateBoard(Move move) const
	{
		std::shared_ptr<BoardImpl> newBoard = std::make_shared<BoardImpl>();

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				Piece p = this->m_pieces[i][j];
				newBoard->m_pieces[i][j] = {p.pieceType, p.color };
			}
		}
		newBoard->m_canWhiteCastleLeft = this->m_canWhiteCastleLeft;
		newBoard->m_canWhiteCastleRight = this->m_canWhiteCastleRight;
		newBoard->m_canBlackCastleLeft = this->m_canBlackCastleLeft;
		newBoard->m_canBlackCastleRight = this->m_canBlackCastleRight;
		newBoard->m_whoPlaysNext = (this->m_whoPlaysNext == Color::White ? Color::Black : Color::White);

		Piece pSource = this->m_pieces[move.sourceRank][move.sourceFile];
		Piece pSourceNew = newBoard->m_pieces[move.sourceRank][move.sourceFile];
		Piece pNewTgt = newBoard->m_pieces[move.destinationRank][move.destinationFile];

		pSourceNew = { PieceType::None, Color::White };
		pNewTgt = { pSource.pieceType, pSource.color };


		// Castling bools update
		switch (this->m_whoPlaysNext) {
			case Color::White:
				if (pSource.pieceType == PieceType::King)
					newBoard->m_canWhiteCastleLeft = newBoard->m_canWhiteCastleRight = false;
				else if (pSource.pieceType == PieceType::Rook) {
					if (move.sourceRank == 0) {
						if (move.sourceFile == 0)
							newBoard->m_canWhiteCastleLeft = false;
						else if (move.sourceFile == 7)
							newBoard->m_canWhiteCastleRight = false;
					}
				}
				break;
			case Color::Black:
				if (pSource.pieceType == PieceType::King)
					newBoard->m_canBlackCastleLeft = newBoard->m_canBlackCastleRight = false;
				else if (pSource.pieceType == PieceType::Rook) {
					if (move.sourceRank == 7) {
						if (move.sourceFile == 7)
							newBoard->m_canWhiteCastleLeft = false;
						else if (move.sourceFile == 0)
							newBoard->m_canWhiteCastleRight = false;
					}
				}
		}

		switch (pSource.pieceType) {
		case PieceType::King:
			int fileChange = move.destinationFile - move.sourceFile;
			if (fileChange == 2 || fileChange == -2) {    // castling
				int rookFile = (fileChange > 0 ? 7 : 0);
				Piece pRook = this->m_pieces[move.sourceRank][rookFile];
				// assert ??
				newBoard->m_pieces[move.sourceRank][rookFile].pieceType = PieceType::None;
				int rookDestFile = (move.sourceFile + move.destinationFile) / 2;
				newBoard->m_pieces[move.sourceRank][rookDestFile] = { pRook.pieceType, pRook.color };
			}

			break;
		case PieceType::Pawn:
			if (move.destinationRank == 0 || move.destinationRank == 7) {  // promotion
				pNewTgt.pieceType = move.promotedPiece; 
			}

			switch (pSource.color){
			case Color::White:
				if (move.sourceRank == 1 && move.destinationRank == 3) {  // double move
					pNewTgt.pieceType = PieceType::EnPessantCapturablePawn;
				}
				else if (move.sourceRank == 4 && 
					newBoard->m_pieces[move.sourceRank][move.destinationFile].pieceType 
							== PieceType::EnPessantCapturablePawn){
					newBoard->m_pieces[move.sourceRank][move.destinationFile].pieceType
						== PieceType::None;    // En passant capture
				}
				break;
			case Color::Black:
				if (move.sourceRank == 6 && move.destinationRank == 4) {  // double move
					pNewTgt.pieceType = PieceType::EnPessantCapturablePawn;
				}
				else if (move.sourceRank == 3 &&
					newBoard->m_pieces[move.sourceRank][move.destinationFile].pieceType
					== PieceType::EnPessantCapturablePawn) {
					newBoard->m_pieces[move.sourceRank][move.destinationFile].pieceType
						== PieceType::None;    // En passant capture
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

		int enPassantRow = pSource.color == Color::White ? 4 : 3; // en passant update
		for (int j = 0; j < 8; j++) {
			if (newBoard->m_pieces[enPassantRow][j].pieceType == PieceType::EnPessantCapturablePawn)
				newBoard->m_pieces[enPassantRow][j].pieceType = PieceType::Pawn;
		}
		return newBoard;
	}



	// only valid moves which doesnt put King in check-mate position
	// For castling, we examine all check conditions here
	std::map<Move, IBoard::Ptr> BoardImpl::getPossibleMoves() const
	{
		std::vector<Move> allMoves = this->getAllmovesWithoutObstructions();
		std::map<Move, IBoard::Ptr> movesMap;

		for (const Move& m : allMoves) {
			std::optional<IBoard::Ptr> newBoard = this->updateBoard(m);
			if (newBoard.has_value()) {
				if (newBoard.value()->isCheckMate())
					continue;
				if (this->m_pieces[m.sourceRank][m.sourceFile].pieceType == PieceType::King &&
					abs(m.destinationFile - m.sourceFile) == 2
					) {
					int castledir = (m.sourceFile - m.destinationFile) / 2;
					if (this->isCheckMate(-1,-1,false) ||
						this->isCheckMate(m.sourceRank, m.sourceFile + castledir, false)
						)
						continue;
				}
				movesMap[m] = newBoard.value();

			}
		}
		return movesMap;
	}

	IBoard::Ptr BoardImpl::getStartingBoard()
	{
		std::shared_ptr<BoardImpl> board = std::make_shared<BoardImpl>();
		
		// white pieces
		board->m_pieces[0][0] = { PieceType::Rook, Color::White };
		board->m_pieces[0][1] = { PieceType::Knight, Color::White };
		board->m_pieces[0][2] = { PieceType::Bishop, Color::White };
		board->m_pieces[0][3] = { PieceType::Queen, Color::White };
		board->m_pieces[0][4] = { PieceType::King, Color::White };
		board->m_pieces[0][5] = { PieceType::Bishop, Color::White };
		board->m_pieces[0][6] = { PieceType::Knight, Color::White };
		board->m_pieces[0][7] = { PieceType::Rook, Color::White };
		board->m_canWhiteCastleLeft = board->m_canWhiteCastleRight = true;

		for (std::size_t i = 0; i < 8; ++i)
			board->m_pieces[1][i] = { PieceType::Pawn, Color::White };

		// black pieces
		board->m_pieces[7][0] = { PieceType::Rook, Color::Black };
		board->m_pieces[7][1] = { PieceType::Knight, Color::Black };
		board->m_pieces[7][2] = { PieceType::Bishop, Color::Black };
		board->m_pieces[7][3] = { PieceType::Queen, Color::Black };
		board->m_pieces[7][4] = { PieceType::King, Color::Black };
		board->m_pieces[7][5] = { PieceType::Bishop, Color::Black };
		board->m_pieces[7][6] = { PieceType::Knight, Color::Black };
		board->m_pieces[7][7] = { PieceType::Rook, Color::Black };

		board->m_canBlackCastleLeft = board->m_canBlackCastleRight = true;
		for (std::size_t i = 0; i < 8; ++i)
			board->m_pieces[6][i] = { PieceType::Pawn, Color::Black };

		// empty positions
		for (std::size_t file = 0; file < 8; ++file)
			for (std::size_t rank = 2; rank < 6; ++rank)
				board->m_pieces[rank][file] = { PieceType::None, Color::White };

		// white make the first move
		board->m_whoPlaysNext = Color::White;

		return board;
	}


	IBoard::Ptr BoardImpl::fromFen(const Fen& fen)
	{
		std::shared_ptr<BoardImpl> board = std::make_shared<BoardImpl>();

		for (int rank = 0; rank < 8; ++rank)
			for (int file = 0; file < 8; ++file)
				board->m_pieces[rank][file].pieceType = PieceType::None;


		std::stringstream ss(fen.fen);
		char c;
		for (int rank = 7; rank >= 0; --rank)
		{
			for (int file = 0; file < 8; ++file)
			{
				ss.get(c);
				if (c >= 'a' && c <= 'z')
				{
					// black piece
					Piece piece;
					piece.color = Color::Black;
					piece.pieceType = toPieceType(c);
					board->m_pieces[rank][file] = piece;
				}
				else if (c >= 'A' && c <= 'Z')
				{
					// white piece
					Piece piece;
					piece.color = Color::White;
					piece.pieceType = toPieceType(c);
					board->m_pieces[rank][file] = piece;
				}
				else if (c >= '1' && c <= '8')
				{
					// number of blank squares
					file += (c - '1');
				}
			}
			if (rank > 0)
			{
				ss.get(c);
				if (c != '/') 
					throw std::runtime_error(std::string("Expecting '/', got '") + c + "'");
			}
		}

		ss.get(c);
		if (c != ' ') throw std::runtime_error(std::string("Expecting ' ', got '") + c + "'");

		ss.get(c);
		if (c == 'b') board->m_whoPlaysNext = Color::Black;
		else if (c == 'w') board->m_whoPlaysNext = Color::White;
		else throw std::runtime_error(std::string("Expecting 'b' or 'w', got '") + c + "'");

		ss.get(c);
		if (c != ' ') throw std::runtime_error(std::string("Expecting ' ', got '") + c + "'");

		board->m_canBlackCastleLeft = board->m_canBlackCastleRight = board->m_canWhiteCastleLeft = board->m_canWhiteCastleRight = false;
		for (ss.get(c); c != ' '; ss.get(c))
		{
			switch (c) {
			case 'K':
				board->m_canWhiteCastleRight = true;
				break;
			case 'k':
				board->m_canBlackCastleLeft = true;
				break;
			case 'Q':
				board->m_canWhiteCastleLeft = true;
				break;
			case 'q':
				board->m_canBlackCastleRight = true;
				break;
			case '-':
				break;
			default:
				throw std::runtime_error(std::string("Expecting one of KkQq-, got '") + c + "'");
			}
		}

		ss.get(c);
		if (c >= 'a' && c <= 'h')
		{
			int rank = c - 'a';
			ss.get(c);
			int file;
			if (c >= '1' && c <= '8')
				file = c - '1';
			else
				throw std::runtime_error(std::string("Expecting a digit from 1 to 8, got '") + c + "'");
			if (board->m_whoPlaysNext == Color::Black)
				rank += 1;
			board->m_pieces[rank][file].pieceType = PieceType::EnPessantCapturablePawn;
		}
		else if (c != '-') throw std::runtime_error(std::string("Expecting 'a'-'h' or '-', got '") + c + "'");

		return board;
	}
  
	// In this we check for obstructions, returns true if no obstructions
	bool BoardImpl::checkObstructions(Move m) const
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


	std::vector<Move> BoardImpl::getAllmoves(bool current = true) const
	{
		std::vector<Move> allMoves;

		for(int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::vector<Move> moves = this->getAllmoves(i, j, current);
				allMoves.insert(allMoves.end(), moves.begin(), moves.end());
			}
		return allMoves;
	}

	std::vector<Move> BoardImpl::getAllmovesWithoutObstructions(bool current = true) const
	{
		std::vector<Move> allMoves = this->getAllmoves(current);
		std::vector<Move> goodMoves;
		for (const Move& m : allMoves) {
			if (this->checkObstructions(m)) {
				goodMoves.push_back(m);
			}
		}
		return goodMoves;
	}
	


	std::vector<Move> BoardImpl::getAllmoves(int rank, int file, bool current = true) const
	{
		Piece p = this->m_pieces[rank][file];
		PieceType t = p.pieceType;
		Color c = p.color;
		std::vector<Move> moves;

		Color moveColor = getColor(current);

		if (t == PieceType::None || c != moveColor) {
			return std::vector<Move>();
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

		return moves;
	}

	
}
