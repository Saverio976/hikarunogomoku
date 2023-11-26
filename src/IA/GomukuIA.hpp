//
// Created by Théo on 10/11/2023.
//

#pragma once

#include "GomukuBoard.hpp"
#include <vector>
#include <utility>
#include <cmath>
#include <functional>

class GomukuAI {
public:
    GomukuAI(int depth);
    std::pair<int, int> findBestMove(GomukuBoard& board);
private:
    int _maxDepth;

    std::pair<int, std::pair<int, int>> findBestMoveThread(GomukuBoard &board, int depth, const std::vector<std::pair<int, int>> &moves);

    inline int evaluateBoard(const GomukuBoard&);

    int evaluateDirection(GomukuBoard board, int x, int y, int dx, int dy);

    bool isInBounds(int x, int y);

    int maxValue(GomukuBoard &board, int depth, int alpha, int beta);

    int minValue(GomukuBoard &board, int depth, int alpha, int beta);

    std::unordered_map<uint64_t, int> _transpositionTable;
    std::mutex _transpositionTableMutex;
    int _previousDepth = 4;
};
