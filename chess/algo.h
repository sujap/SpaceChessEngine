#pragma once

#include "board.h"

#include <vector>


namespace space {

    class IAlgo {
    public:
        using Ptr = std::shared_ptr<IAlgo>;
        virtual Move getNextMove(IBoard::Ptr board) = 0;
    };

    class IScoreBasedAlgo : public IAlgo {
    public:
        using Score = double;
        virtual Score getScore(IBoard::Ptr board) = 0;
    };

    class IGame {
    public:
        using MoveList = std::vector<Move>;

        virtual IAlgo::Ptr getBlackAlgo() const = 0;
        virtual IAlgo::Ptr getWhiteAlgo() const = 0;
        virtual IBoard::Ptr getCurrentBoard() const = 0;
        virtual MoveList getMovesSoFar() = 0;

        IBoard::Ptr makeNextMove(Move move);
        Move getNextMove();
    };

}









