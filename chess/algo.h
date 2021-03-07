#pragma once

#include "board.h"

#include <vector>


namespace space {

    class IAlgo {
    public:
        using Ptr = std::shared_ptr<IAlgo>;
        virtual Move getNextMove(IBoard::Ptr board) = 0;
    };

}









