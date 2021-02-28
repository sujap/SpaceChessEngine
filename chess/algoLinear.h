#pragma once

#include "board.h"
#include "algo.h"

#include <string>
#include <vector>

namespace space {

   
    class IAlgoLinear : public IScoreBasedAlgo {
    public:
        IAlgoLinear(std::vector<double> wtsVec);
        std::map<PieceType, double> weights;
        Score getLinearScore(IBoard::Ptr board);

    private:

    };


    class IAlgoMultiLinear : public IScoreBasedAlgo {
    public:
        //TODO constructor
        virtual int algoChoice(IBoard::Ptr board) = 0;
        using wtsMap = std::map<PieceType, double>;
        std::vector<wtsMap> algoList;

    private:

    };

    class AlgoLinearDepthOne : public IAlgoLinear {
    public:
        Score getScore(IBoard::Ptr board) override;

    private:
        

    };
    class AlgoLinearDepthTwoDef : public IAlgoLinear {
    public:
        Score getScore(IBoard::Ptr board) override;
        int breadthCount;
        Move getNextMove(IBoard::Ptr board) override;

    private:

    };



}

