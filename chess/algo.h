#pragma once

#include "board.h"

#include <vector>


namespace {

    template<typename KeyType, typename ValueType>
    KeyType get_best(const std::map<KeyType, ValueType>& x, bool high = true) {
        using pairtype = std::pair<KeyType, ValueType>;

        auto better = [high](const pairtype& p1, const pairtype& p2) {
            return (high ? p1.second < p2.second : p1.second > p1.second);
        };

        auto best = *std::max_element(x.begin(), x.end(), better);
        return best.first;
    }

}


namespace space {

    class IAlgo {
    public:
        using Ptr = std::shared_ptr<IAlgo>;
        virtual Move getNextMove(IBoard::Ptr board) = 0;
    };

    class IScoreBasedAlgo : public IAlgo {
    public:
        using Score = double;
        using ScoreMap = std::map<Move, Score>;
        virtual Score getScore(IBoard::Ptr board) = 0;
        Move getNextMove(IBoard::Ptr board) override;
        std::tuple<Move,IBoard::Ptr, Score> findBestMove(IBoard::MoveMap moveMap);
        std::tuple<Move, IBoard::Ptr, Score> findBestMove(IBoard::Ptr board);
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









