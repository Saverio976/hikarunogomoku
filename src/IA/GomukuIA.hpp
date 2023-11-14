//
// Created by Théo on 10/11/2023.
//

#pragma once

#include "GomukuBoard.hpp"
#include "PatterMatcher.hpp"
#include "Pattern20.hpp"
#include <vector>

class GomukuAI {
public:
    GomukuAI(int depth);
    std::pair<int, int> findBestMove(GomukuBoard& board);
private:
    int maxDepth;
    inline int maximize(GomukuBoard& board, int depth, int alpha, int beta);
    inline int minimize(GomukuBoard& board, int depth, int alpha, int beta);
    inline int evaluateBoard(const GomukuBoard& board);
    std::vector<std::pair<PatternMatcher, int>> patternMatchers;
};
