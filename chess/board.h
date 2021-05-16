#pragma once

#include <memory>
#include <string>
#include <optional>
#include <map>
#include <string>
#include <sstream>

namespace space {
	enum class Color { White, Black };
	enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King, None };

  int colorToSign(Color);
	PieceType charToPieceType(char c);
	char pieceTypeToChar(PieceType p);
  
  
    struct Position {
		int rank;
		int file;
		Position(int v_rank, int v_file) : rank(v_rank), file(v_file) {}
		Position(std::string san) : Position(san[1] - '1', san[0] - 'a') { }
	};

	struct Move {
		int sourceRank;
		int sourceFile;
		int destinationRank;
		int destinationFile;
		PieceType promotedPiece; // for pawn Promotion only
		Move() {}
		Move(int v_sourceRank, int v_sourceFile, int v_destinationRank, int v_destinationFile, PieceType v_promotedPiece = PieceType::None) :
			sourceRank(v_sourceRank), sourceFile(v_sourceFile), destinationRank(v_destinationRank), destinationFile(v_destinationFile), promotedPiece(v_promotedPiece)
		{}
		Move(const std::string &s);

		std::string toString() const;
		bool operator <(const Move& that) const;

        inline std::string as_string() const {
			std::stringstream ss;
			ss << (char)('a' + sourceFile) << (sourceRank + 1)
			   << " -> "
			   << (char)('a' + destinationFile) << (destinationRank + 1);
			return ss.str();
		}
	};
  
	struct Piece {
		PieceType pieceType;
		Color color;
		Piece() {}
		Piece(PieceType _p, Color _c) : pieceType(_p), color(_c) {}
		Piece(char c);
		char as_char(bool color = true) const;
		std::string as_unicode() const;
  };

  class IBoard {
	public:
		using Ptr = std::shared_ptr<IBoard>;
		using MoveMap = std::map<Move, Ptr>;
		virtual Color whoPlaysNext() const = 0;
		virtual std::optional<Piece> getPiece(Position position) const = 0;

		std::optional<Position> enPassantSquare;

		// (Left and right from that player's point of view)
		virtual bool canCastleLeft(Color color) const = 0;
		virtual bool canCastleRight(Color color) const = 0;

		virtual bool isStaleMate() const = 0;
		virtual bool isCheckMate() const = 0;
		virtual bool isUnderCheck(
			Color color,
			std::optional<Position> targetKingPosition = std::nullopt
		) const = 0;
		virtual std::optional<Ptr> updateBoard(Move move) const = 0;
		virtual MoveMap getValidMoves() const = 0;
		virtual std::string as_string(
				bool unicode_pieces = false,
				bool terminal_colors = false,
				Color perspective = Color::White
		) const = 0;
	};

	std::string moveToString(Move m, IBoard::Ptr board);

}
