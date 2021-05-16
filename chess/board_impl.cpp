#include "board_impl.h"
#include "common/base.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <cmath>

namespace {

	
	std::string board_as_plain_string(
		const space::IBoard& board,
		bool unicode_pieces,
		space::Color perspective)
	{
		std::stringstream out;
		out << "  |  a  b  c  d  e  f  g  h  |\n"
			<< "--+--------------------------+--\n";
		for (int rank = 7; rank >= 0; --rank)
		{
			out << (rank + 1) << " |  ";
			for (int file = 0; file < 8; ++file)
			{
				auto piece = board.getPiece({ rank, file });
				if (piece.has_value()) {
					if (unicode_pieces) {
						out << piece.value().as_unicode() << "  ";
					}
					else {
						auto c = piece.value().pieceType != space::PieceType::None
						    ? piece.value().as_char()
							: ' ';
						out << c << "  ";
					}
				}
				else {
					out << ".  ";
				}
			}
			out << "| " << (rank + 1) << "\n";
			if (rank > 0)
				out << "  |                          |  \n";
		}
		out << "--+--------------------------+--\n"
			<< "  |  a  b  c  d  e  f  g  h  |\n\n";
		return out.str();
	}
} // end anonymous namespace


namespace space {

	// CLASS BoardImpl

	Color BoardImpl::whoPlaysNext() const
	{
		return m_whoPlaysNext;
	}

	Color BoardImpl::getColor(bool current) const{
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
	// No piece in between -- examined in checkObstructions
	// No check in 3 cells // examined in getPossibleMoves

	bool BoardImpl::canCastleLeft(Color color) const
	{
		switch (color)
		{
		case Color::White:
			return this->m_canWhiteCastleLeft;
		
		case Color::Black:
			return this->m_canBlackCastleLeft;

		default:
			throw std::runtime_error("Invalid color");
		}
	}
	bool BoardImpl::canCastleRight(Color color) const
	{
		switch (color)
		{
		case Color::White:
			return this->m_canWhiteCastleRight;

		case Color::Black:
			return this->m_canBlackCastleRight;

		default:
			throw std::runtime_error("Invalid color");
		}
	}
	

	Position space::BoardImpl::getKingPosition(Color color) const
	{

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				Piece p = this->m_pieces[i][j];
				if (p.color == color && p.pieceType == PieceType::King) {
					return Position(i, j);
				}
			}
		}
	}


	bool BoardImpl::isStaleMate() const
	{
		if (!this->isUnderCheck(this->m_whoPlaysNext)) {
			std::map<Move, IBoard::Ptr> allMoves = this->getValidMoves();
			return allMoves.size() == 0;
		}
		return false;
	}

	bool BoardImpl::isCheckMate() const
	{
		if (this->isUnderCheck(this->m_whoPlaysNext)) {
			std::map<Move, IBoard::Ptr> allMoves = this->getValidMoves();
			return allMoves.size() == 0;
		}
		return false;
	}

	
	std::optional<IBoard::Ptr> BoardImpl::updateBoard(Move move) const
	{
		std::shared_ptr<BoardImpl> newBoard = std::make_shared<BoardImpl>();

		newBoard->m_pieces = this->m_pieces;
		newBoard->m_canWhiteCastleLeft = this->m_canWhiteCastleLeft;
		newBoard->m_canWhiteCastleRight = this->m_canWhiteCastleRight;
		newBoard->m_canBlackCastleLeft = this->m_canBlackCastleLeft;
		newBoard->m_canBlackCastleRight = this->m_canBlackCastleRight;
		newBoard->m_whoPlaysNext = this->getColor(false);
		newBoard->enPassantSquare = {};

		Piece pSource = this->m_pieces[move.sourceRank][move.sourceFile];
		Piece& pSourceNew = newBoard->m_pieces[move.sourceRank][move.sourceFile];
		Piece& pTargetNew = newBoard->m_pieces[move.destinationRank][move.destinationFile];

		pSourceNew = { PieceType::None, Color::White };
		pTargetNew = { pSource.pieceType, pSource.color };


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

				// If white captures (rook or not) on h8 or a8, black loses right
				// to castle.
				if (move.destinationRank == 7 && move.destinationFile == 0) {
					newBoard->m_canBlackCastleRight = false;
				}
				else if (move.destinationRank == 7 && move.destinationFile == 7) {
					newBoard->m_canBlackCastleLeft = false;
				}

				space_assert(!newBoard->m_canWhiteCastleLeft || newBoard->m_pieces[0][0].pieceType == PieceType::Rook,
					   "White Left Rook moved, cant castle");
				space_assert(!newBoard->m_canWhiteCastleRight || newBoard->m_pieces[0][7].pieceType == PieceType::Rook,
					"White Right Rook moved, cant castle");
				break;
			case Color::Black:
				if (pSource.pieceType == PieceType::King)
					newBoard->m_canBlackCastleLeft = newBoard->m_canBlackCastleRight = false;
				else if (pSource.pieceType == PieceType::Rook) {
					if (move.sourceRank == 7) {
						if (move.sourceFile == 7)
							newBoard->m_canBlackCastleLeft = false;
						else if (move.sourceFile == 0)
							newBoard->m_canBlackCastleRight = false;
					}
				}

				// If white captures (rook or not) on h8 or a8, black loses right
				// to castle.
				if (move.destinationRank == 0 && move.destinationFile == 0) {
					newBoard->m_canWhiteCastleLeft = false;
				}
				else if (move.destinationRank == 0 && move.destinationFile == 7) {
					newBoard->m_canWhiteCastleRight = false;
				}


				space_assert(!newBoard->m_canBlackCastleLeft || newBoard->m_pieces[7][7].pieceType == PieceType::Rook,
					"Black Left Rook moved, cant castle");
				space_assert(!newBoard->m_canBlackCastleRight || newBoard->m_pieces[7][0].pieceType == PieceType::Rook,
					"Black Right Rook moved, cant castle");
		}

		// all other updates
		int fileChange = move.destinationFile - move.sourceFile;
		switch (pSource.pieceType) {		
		case PieceType::King:
			if (fileChange == 2 || fileChange == -2) {    // castling
				int rookFile = (fileChange > 0 ? 7 : 0);
				Piece pRook = this->m_pieces[move.sourceRank][rookFile];
				space_assert(pRook.pieceType == PieceType::Rook &&
						pRook.color == pSource.color,
					"Rook Not found for castling");

				newBoard->m_pieces[move.sourceRank][rookFile].pieceType = PieceType::None;
				int rookDestFile = (move.sourceFile + move.destinationFile) / 2;
				newBoard->m_pieces[move.sourceRank][rookDestFile] = { pRook.pieceType, pRook.color };
			}
			break;

		case PieceType::Pawn:
			space_assert(abs(fileChange) <= 1,
				"Pawn move too far");

			if (move.destinationRank == 0 || move.destinationRank == 7) {  // promotion
				space_assert((move.destinationRank == 0) == (pSource.color == Color::Black),
					"Pawn on back row");
				pTargetNew.pieceType = move.promotedPiece;
			}

			switch (pSource.color){
			case Color::White:
				if (move.sourceRank == 1 && move.destinationRank == 3) {  // double move
					space_assert(this->m_pieces[2][move.sourceFile].pieceType == PieceType::None,
						"White Pawn double move blocked");
					pTargetNew.pieceType = PieceType::Pawn;
					newBoard->enPassantSquare = { 2, move.sourceFile };
				}
				else if (move.sourceRank == 4 && enPassantSquare.has_value()
				      && move.destinationRank == enPassantSquare.value().rank
					  && move.destinationFile == enPassantSquare.value().file) {
					newBoard->m_pieces[move.sourceRank][move.destinationFile].pieceType
						= PieceType::None;    // En passant capture
				}
				break;
			case Color::Black:
				if (move.sourceRank == 6 && move.destinationRank == 4) {  // double move
					space_assert(this->m_pieces[5][move.sourceFile].pieceType == PieceType::None,
						"Black Pawn double move blocked");
					pTargetNew.pieceType = PieceType::Pawn;
					newBoard->enPassantSquare = { 5, move.sourceFile };
				}
				else if (move.sourceRank == 3 && enPassantSquare.has_value()
				      && move.destinationRank == enPassantSquare.value().rank
					  && move.destinationFile == enPassantSquare.value().file) {
					newBoard->m_pieces[move.sourceRank][move.destinationFile].pieceType
						= PieceType::None;    // En passant capture
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

		return newBoard;
	}



	// only valid moves which doesnt put King in check-mate position
	// For castling, we examine all check conditions here
	BoardImpl::MoveMap BoardImpl::getValidMoves() const
	{
		Color color = this->getColor(true);

		std::vector<Move> allMoves = this->getAllmovesWithoutObstructions(color);
		MoveMap movesMap;

		for (const Move& m : allMoves) {
			std::optional<IBoard::Ptr> newBoardPtr = this->updateBoard(m);
			if (!newBoardPtr.has_value()) {
				continue;
			}
			BoardImplPtr newBoard = std::dynamic_pointer_cast<BoardImpl>(newBoardPtr.value());
			if (newBoard->isUnderCheck(color)) // CHECK
				continue;
			if (this->m_pieces[m.sourceRank][m.sourceFile].pieceType == PieceType::King &&
				abs(m.destinationFile - m.sourceFile) == 2   //Castling
				) {
				int castledir = (m.destinationFile - m.sourceFile) / 2;
				if (this->isUnderCheck(color) ||
					this->isUnderCheck(color, Position(m.sourceRank, m.sourceFile + 2*castledir)) ||
					this->isUnderCheck(color, Position(m.sourceRank, m.sourceFile + castledir))
					)
					continue;
			}
			movesMap[m] = newBoard;
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
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
				{
					// piece
					board->m_pieces[rank][file] = Piece(c);
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
			int file = c - 'a';
			ss.get(c);
			int rank;
			if (c >= '1' && c <= '8')
				rank = c - '1';
			else
				throw std::runtime_error(std::string("Expecting a digit from 1 to 8, got '") + c + "'");
			int pawnRank = board->m_whoPlaysNext == Color::White ? 4 : 3;
			space_assert(
				board->m_pieces[pawnRank][file].pieceType == PieceType::Pawn,
				"En passant capture square does not have a pawn in front of it."
			);
			board->enPassantSquare = { rank, file };
		}
		else if (c == '-') {
			board->enPassantSquare = {};
		}
		else {
			throw std::runtime_error(std::string("Expecting 'a'-'h' or '-', got '") + c + "'");
		}

		return board;
	}


	BoardImpl::BoardImpl() {}


	BoardImpl::BoardImpl(
			const std::array<std::array<Piece, 8>, 8> & pieces,
			bool canWhiteCastleLeft,
			bool canWhiteCastleRight,
			bool canBlackCastleLeft,
			bool canBlackCastleRight,
			Color whoPlaysNext):
		m_pieces(pieces),
		m_canWhiteCastleLeft(canWhiteCastleLeft),
		m_canWhiteCastleRight(canWhiteCastleRight),
		m_canBlackCastleLeft(canBlackCastleLeft),
		m_canBlackCastleRight(canBlackCastleRight),
		m_whoPlaysNext(whoPlaysNext)
	{ }


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

		if (pType == PieceType::Pawn) {
			return true;
		}
		
		Piece pTarget = this->m_pieces[m.destinationRank][m.destinationFile];
		PieceType pTargetType = pTarget.pieceType;
		Color ct = pTarget.color;
		
		if (pTargetType != PieceType::None && ct == c)
		{
			return false; // target cell obstruction by same color
		}

		int deltax = m.destinationRank - m.sourceRank;
		int deltay = m.destinationFile - m.sourceFile;

		// path obstruction calculation
		if (pType == PieceType::Rook || pType == PieceType::Bishop || pType == PieceType::Queen)
		{
			space_assert(deltax == 0 || deltay == 0 || abs(deltax) == abs(deltay),
				"invalide long  move");

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
		// castling obstruction check
		else if(pType == PieceType::King && abs(deltax) == 2) {
			space_assert(deltay == 0,
				"King moving too far");
			int direction = deltax / 2;
			int rookDistance = deltax < 0 ? 4 : 3;
			for (int j = 1; j < rookDistance; j++) {
				if (m_pieces[m.sourceRank][m.sourceFile + j * direction].pieceType != PieceType::None) {
					return false;
				}
			}
		}
		return true;
	}


	// tell me whether in the current board position, the king of the specified color is under threat or not
	// if position specified, examines for king moving to that cell (useful in castling checks)
	bool BoardImpl::isUnderCheck(Color color, std::optional<Position> targetKingPosition) const
	{
		Position base_position = targetKingPosition.has_value() ?
			targetKingPosition.value() 
			: getKingPosition(color);

		Color oppColor = color == Color::Black ? Color::White : Color::Black;
		int rank = base_position.rank;
		int file = base_position.file;

		for (auto& direction: internals::MoveOffsets::orthogonal_offsets) {
			auto piece = internals::Utils::get_first_piece(this, base_position, direction);
			if (piece.has_value() && piece.value().color == oppColor
			    && (piece.value().pieceType == PieceType::Rook ||
			        piece.value().pieceType == PieceType::Queen))
				return true;
		}

		for (auto& direction: internals::MoveOffsets::diagonal_offsets) {
			auto piece = internals::Utils::get_first_piece(this, base_position, direction);
			if (piece.has_value() && piece.value().color == oppColor
			    && (piece.value().pieceType == PieceType::Bishop ||
			        piece.value().pieceType == PieceType::Queen)) 
				return true;
		}

		for (auto& direction: internals::MoveOffsets::king_offsets) {
			auto piece = internals::Utils::get_first_piece(this, base_position, direction);
			if (piece.has_value() && piece.value().color == oppColor
			    && piece.value().pieceType == PieceType::King)
				return true;
		}

		for (auto& direction: internals::MoveOffsets::knight_offsets) {
			auto piece = internals::Utils::get_first_piece(this, base_position, direction);
			if (piece.has_value() && piece.value().color == oppColor
			    && piece.value().pieceType == PieceType::Knight)
				return true;
		}

		auto direction = oppColor == Color::White ? 1 : -1;
		auto pawn_rank = rank - direction;
		if (pawn_rank >= 0 && pawn_rank <= 7) {
			for (auto file_offset: { 1, -1}) {
				auto pawn_file = file_offset + file;
				if (pawn_file < 0 || pawn_file > 7) continue;
				auto pawn = m_pieces[pawn_rank][pawn_file];
				if (pawn.pieceType == PieceType::Pawn && pawn.color == oppColor) return true;
			}
		}
		return false;
	}

	//TODO
	// checks if all in-between cells for a move are empty
	bool BoardImpl::checkPathEmpty(Move m) const
	{
		bool diag = (m.destinationFile != m.sourceFile) && (m.destinationRank != m.sourceRank);


		int deltax = m.destinationRank - m.sourceRank;
		int deltay = m.destinationFile - m.sourceFile;
		int delta = std::max(abs(deltax), abs(deltay));
		int sgnx = deltax > 0 ? 1 : (deltax < 0 ? -1 : 0);
		int sgny = deltay > 0 ? 1 : (deltay < 0 ? -1 : 0);

		for (int j = 1; j < delta; j++)
		{
			int rank = m.sourceRank + j * sgnx;
			int file = m.sourceFile + j * sgny;
			if (this->m_pieces[rank][file].pieceType != PieceType::None)
			{
				return false;
			}
		}
		return true;
	}




	// TODO: check if a move is possible 
	// ignoring : 
	//		who plays next 
	//		checkmate
	//		zero move
	//		castling
	bool BoardImpl::canMove(Move m) const
	{
		Piece pSource = this->m_pieces[m.sourceRank][m.sourceFile];
		Piece pTarget = this->m_pieces[m.destinationRank][m.destinationFile];

		// moving to same color
		if ((pTarget.pieceType != PieceType::None)&&
			pTarget.color == pSource.color)
			return false;

		switch (pSource.pieceType)
		{
		case PieceType::None:
			return false;

		case PieceType::Pawn:
		{
			int direction = colorToSign(pSource.color);
			return ((m.destinationFile == m.sourceFile) &&
				(m.destinationRank == m.sourceRank + direction) &&
				(pTarget.pieceType == PieceType::None))

				|| ((abs(m.destinationFile - m.sourceFile) == 1) &&
					(m.destinationRank == m.sourceRank + direction) &&
					(pTarget.color != pSource.color));
		}

		case PieceType::Bishop:
			return (abs(m.destinationFile - m.sourceFile) ==  abs(m.destinationRank - m.sourceRank)) 
				&& checkPathEmpty(m);

		case PieceType::Knight:
		{
			auto f = [](int a, int b) {
				return (a == 1 && b == 2) || (a == 2 && b == 1); };
			return f(abs(m.destinationFile - m.sourceFile),
				abs(m.destinationRank - m.sourceRank));
		}

		case PieceType::Rook:
			return ((m.destinationFile == m.sourceFile) ||
					(m.destinationRank == m.sourceRank)) &&
				checkPathEmpty(m);

		case PieceType::Queen:
			return ((m.destinationFile == m.sourceFile) ||
					(m.destinationRank == m.sourceRank) ||
					abs(m.destinationFile - m.sourceFile) == 
								abs(m.destinationRank - m.sourceRank)
					) &&
				checkPathEmpty(m);

		case PieceType::King:
			return	abs(m.destinationFile - m.sourceFile) <= 1 &&
				abs(m.destinationRank - m.sourceRank) <= 1;			
		}

	}

	std::vector<Move> BoardImpl::getAllMoves(Color color) const
	{
		std::vector<Move> allMoves;

		for(int rank = 0; rank < 8; rank++)
			for (int file = 0; file < 8; file++) {
				Piece piece = this->m_pieces[rank][file];
				if (piece.color == color && piece.pieceType != PieceType::None) {
					std::vector<Move> moves = this->getAllMoves({ rank, file });
					allMoves.insert(allMoves.end(), moves.begin(), moves.end());
				}
			}
		return allMoves;
	}

	std::vector<Move> BoardImpl::getAllmovesWithoutObstructions(Color color) const
	{
		std::vector<Move> allMoves = this->getAllMoves(color);
		std::vector<Move> goodMoves;
		for (const Move& m : allMoves) {
			if (this->checkObstructions(m)) {
				goodMoves.push_back(m);
			}
		}
		return goodMoves;
	}
	


	std::vector<Move> BoardImpl::getAllMoves(Position position) const
	{
		int rank = position.rank;
		int file = position.file;
		Piece p = this->m_pieces[rank][file];
		PieceType t = p.pieceType;
		Color c = p.color;
		std::vector<Move> moves;

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
			auto enPassantRank = direction == 1 ? 4 : 3;
			if (t == PieceType::Pawn && enPassantSquare.has_value() &&
				rank == enPassantRank && abs(file - enPassantSquare.value().file) == 1) {
				moves.push_back({ rank, file, enPassantSquare.value().rank, enPassantSquare.value().file});
			}
		}

		// Castling: denoted by king moving two steps in left/right

		int direction = (c == Color::White) ? 1 : -1;
		int baseRank = (c == Color::White) ? 0 : 7;

		if (t == PieceType::King && canCastleLeft(c))
		{
			// Everything between king and rook should be clear.
			auto rookFile = (c == Color::White) ? 0 : 7;
			auto clear = true;
			for (auto i = std::min(rookFile, 4) + 1; i < std::max(rookFile, 4); i++) {
				if (m_pieces[baseRank][i].pieceType != PieceType::None) clear = false;
			}
			if (clear)
				moves.push_back({ rank, file, rank, file - 2 * direction });
		}

		if (t == PieceType::King && canCastleRight(c))
		{
			auto rookFile = (c == Color::White) ? 7 : 0;
			auto clear = true;
			for (auto i = std::min(rookFile, 4) + 1; i < std::max(rookFile, 4); i++) {
				if (m_pieces[baseRank][i].pieceType != PieceType::None) clear = false;
			}
			if (clear)
			moves.push_back({ rank, file, rank, file + 2 * direction });
		}

		return moves;
	}


	std::string BoardImpl::as_string(
			bool terminal_colors,
			bool unicode_pieces,
			Color perspective
	) const {
		if (!terminal_colors)
			return board_as_plain_string(*this, unicode_pieces, perspective);
		int start = perspective == Color::White ? 7 : 0;
		int stop = perspective == Color::White ? -1 : 8;
		int step = start > stop ? -1 : 1;

		auto white_square = terminal_colors ? "\u001b[46m" : "";
		auto black_square = terminal_colors ? "\u001b[45m" : "";
		auto reset = "\u001b[0m";

		std::stringstream ss;
		for (int rank = start; rank != stop; rank += step) {
			ss << " " << (rank + 1) << " ";
			for (int file = 7 - start; file != 7 - stop; file -= step) {
				auto square_color = (rank + file) % 2;
				ss << (square_color == 1 ? white_square : black_square);

				auto piece = m_pieces[rank][file];
				if (unicode_pieces) {
					ss << "\u001b[30m" << piece.as_unicode();
				}
				else {
					ss << (piece.pieceType != PieceType::None ? piece.as_char() : ' ');
				}
				ss << ' ' << reset;
			}
			ss << std::endl;
		}
		ss << (perspective == Color::White ? "   a b c d e f g h" : "   h g f e d c b a")
		   << std::endl;

		return ss.str();
	}
}

namespace space::internals {
	// Offsets are structured as a vector of vectors.
	// Starting from a base position, for each vector in offsets, we just need
	// to look the first piece along the vector.
	// E.g., for the orthogonal_offsets, we have 4 vectors corresponding the
	// directions east, west, north, and south.
	// To find a piece that can attack/move to the base position, we look
	// the first piece along each vector, i.e., each direction.
	const std::vector<std::vector<std::pair<int, int>>> MoveOffsets::orthogonal_offsets = {
		{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 0, 5 }, { 0, 6 }, { 0, 7 } },
		{ { 0,-1 }, { 0,-2 }, { 0,-3 }, { 0,-4 }, { 0,-5 }, { 0,-6 }, { 0,-7 } },
		{ { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 } },
		{ {-1, 0 }, {-2, 0 }, {-3, 0 }, {-4, 0 }, {-5, 0 }, {-6, 0 }, {-7, 0 } },
	};
	const std::vector<std::vector<std::pair<int, int>>> MoveOffsets::diagonal_offsets = {
		{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } },
		{ { 1,-1 }, { 2,-2 }, { 3,-3 }, { 4,-4 }, { 5,-5 }, { 6,-6 }, { 7,-7 } },
		{ {-1,-1 }, {-2,-2 }, {-3,-3 }, {-4,-4 }, {-5,-5 }, {-6,-6 }, {-7,-7 } },
		{ {-1, 1 }, {-2, 2 }, {-3, 3 }, {-4, 4 }, {-5, 5 }, {-6, 6 }, {-7, 7 } },
	};
	const std::vector<std::vector<std::pair<int, int>>> MoveOffsets::knight_offsets = {
		{ { 1, 2 } },
		{ { 1,-2 } },
		{ {-1, 2 } },
		{ {-1,-2 } },
		{ { 2, 1 } },
		{ { 2,-1 } },
		{ {-2, 1 } },
		{ {-2,-1 } },
	};
	const std::vector<std::vector<std::pair<int, int>>> MoveOffsets::king_offsets = {
		{ {-1, 1 } },
		{ {-1, 0 } },
		{ {-1,-1 } },
		{ { 0, 1 } },
		{ { 0,-1 } },
		{ { 1, 1 } },
		{ { 1, 0 } },
		{ { 1,-1 } },
	};

	std::optional<Piece> Utils::get_first_piece(
		const BoardImpl* board,
		const Position base_position,
		const std::vector<std::pair<int, int>>& offsets
	) {
		for (auto offset: offsets) {
			auto position = Position(
				base_position.rank + offset.first,
				base_position.file + offset.second
			);
			if (position.rank < 0 || position.rank > 7 ||
			    position.file < 0 || position.file > 7) break;
			auto piece = board->getPiece(position);
			if (piece.has_value()) return piece;
		}

		return {};
	}
}
