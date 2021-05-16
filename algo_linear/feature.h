#pragma once


#include "chess/board.h"
#include "chess/algo.h"



namespace space {

	class Feature {
	public:
		using Ptr = std::shared_ptr<Feature>;
		virtual double getValue(IBoard::Ptr board, Color color) = 0;
		virtual std::string toString()=0;

		// virtual ~Feature() = 0;   //  -- gives LNK2019, CHK
		
		//bool operator <(const Feature& that) const;
	};

	class Feature_Piece : public Feature {
	public:
		Feature_Piece(PieceType p) : piecetype(p) {}
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;

	private:
		PieceType piecetype;

	};

	class Feature_PawnRank : public Feature {
	public:
		Feature_PawnRank(int v_file) : file(v_file) {}
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;

	private:
		int file;
	};

	
	class Feature_PassedPawn : public Feature {
	public:
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;
	
	};

	class Feature_MinorBalance : public Feature {
	public:
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;
	};

	class Feature_Dummy : public Feature {
	public:
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;

	};
	

	class Feature_MoveCount : public Feature {
	public:
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;
	};


	class Feature_PieceMove : public Feature {
	public :
		double getValue(IBoard::Ptr board, Color color) override;
		std::string toString() override;
	};




}
