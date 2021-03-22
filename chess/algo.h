#pragma once

#include "board.h"

#include <vector>
#include <algorithm>


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
        virtual ~IAlgo() noexcept {}
        virtual Move getNextMove(IBoard::Ptr board) = 0;
    };


}









