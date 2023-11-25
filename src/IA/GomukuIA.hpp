//
// Created by Théo on 10/11/2023.
//

#pragma once

#include "GomukuBoard.hpp"
#include <vector>
#include <utility>
#include <cmath>
#include <functional>

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
    int _maxDepth;
    std::unordered_map<ScoreKey, int> _scoreLookupTab;

    std::pair<int, std::pair<int, int>> findBestMoveThread(GomukuBoard &board, int depth, const std::vector<std::pair<int, int>> &moves);

    inline int evaluateBoard(const GomukuBoard&);

    int evaluateDirection(GomukuBoard board, int x, int y, int dx, int dy, bool isPlayer);

    bool isInBounds(int x, int y);

    int maxValue(GomukuBoard &board, int depth, int alpha, int beta);

    int minValue(GomukuBoard &board, int depth, int alpha, int beta);
};
