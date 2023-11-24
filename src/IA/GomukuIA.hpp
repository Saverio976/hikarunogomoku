//
// Created by Théo on 10/11/2023.
//

#pragma once

#include "GomukuBoard.hpp"
#include <vector>
#include <utility>
#include <cmath>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <atomic>

struct ScoreKey {
    int alignedStones;
    int openEnds;

    ScoreKey(int aligned, int open) : alignedStones(aligned), openEnds(open) {}

    bool operator==(const ScoreKey& other) const {
        return alignedStones == other.alignedStones && openEnds == other.openEnds;
    }
};

namespace std {
    template<>
    struct hash<ScoreKey> {
        size_t operator()(const ScoreKey& k) const {
            size_t h1 = std::hash<int>()(k.alignedStones);
            size_t h2 = std::hash<int>()(k.openEnds);
            return h1 ^ (h2 << 1);
        }
    };
}

class GomukuAI {
public:
    GomukuAI(int depth);
    std::pair<int, int> findBestMove(GomukuBoard& board);
private:
    int maxDepth;

    std::atomic<int> bestScore;
    std::pair<int, int> bestMove;
    std::mutex bestMoveMutex;
    std::condition_variable cv;
    bool timeOut = false;
    std::atomic<int> activeWorkers;
    std::condition_variable workersFinishedCv;

    inline int evaluateBoard(const GomukuBoard&);

    int evaluateDirection(GomukuBoard board, int x, int y, int dx, int dy, bool isPlayer);

    bool isInBounds(int x, int y);

    int maxValue(GomukuBoard &board, int depth, int alpha, int beta);

    int minValue(GomukuBoard &board, int depth, int alpha, int beta);

    void evaluateMoves(GomukuBoard &board, const std::vector<std::pair<int, int>>& moves, size_t start, size_t end, int depth);

    bool didMoveBlockFour(GomukuBoard &board, int x, int y);

};
