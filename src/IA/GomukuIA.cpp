//
// Created by Théo on 10/11/2023.
//

#include "GomukuIA.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"
#include <climits>
#include <cstddef>
#include <random>

GomukuAI::GomukuAI(int depth) : maxDepth(depth) {
    auto patterns = Pattern20::getPatterns();
    for (auto& pattern : patterns) {
        auto matcher = PatternMatcher(pattern.getDataPlayer(), pattern.getDataOpponent(), pattern.getMask());
        patternMatchers.emplace_back(matcher, pattern.getScore());
    }
}

inline int GomukuAI::evaluateBoard(const GomukuBoard &board, bool isPlayer) {
    int score = 0;
    Perfcounter::Counter counter(Perfcounter::PerfType::EVALUATE_BOARD);

    int lowerX = std::max(0, board.minX - 1);
    int lowerY = std::max(0, board.minY - 1);
    int upperX = std::min(BOARD_SIZE - 1, board.maxX + 1);
    int upperY = std::min(BOARD_SIZE - 1, board.maxY + 1);

    for (std::size_t y = lowerY; y <= upperY; ++y) {
        for (std::size_t x = lowerX; x <= upperX; ++x) {
            for (auto& patternMatcher : patternMatchers) {
                patternMatcher.first.set_increment(x, y);
                if (isPlayer) {
                    if (patternMatcher.first.isMatch(board.player, board.opponent)) {
                        score += patternMatcher.second;
                    }
                } else {
                    if (patternMatcher.first.isMatch(board.opponent, board.player)) {
                        score -= patternMatcher.second;
                    }
                }
            }
        }
    }
    return score;
}

inline int GomukuAI::maximize(GomukuBoard& board, int depth, int alpha, int beta) {
    if (depth == maxDepth) {
        return evaluateBoard(board);
    }

    int maxEval = INT_MIN;
    auto possibleMoves = board.getPossibleMoves();

    for (auto &[x, y] : possibleMoves) {
        board.set(x, y, true);
        int eval = minimize(board, depth + 1, alpha, beta);
        board.reset(x, y);
        maxEval = std::max(maxEval, eval);
        alpha = std::max(alpha, eval);
        if (beta <= alpha) {
            return maxEval;
        }
    }
    return maxEval;
}

inline int GomukuAI::minimize(GomukuBoard& board, int depth, int alpha, int beta) {
    if (depth == maxDepth) {
        return evaluateBoard(board, false);
    }

    int minEval = INT_MAX;
    auto possibleMoves = board.getPossibleMoves();

    for (auto &[x, y] : possibleMoves) {
        board.set(x, y, false);
        int eval = maximize(board, depth + 1, alpha, beta);
        board.reset(x, y);
        minEval = std::min(minEval, eval);
        beta = std::min(beta, eval);
        if (beta <= alpha) {
            return minEval;
        }
    }
    return minEval;
}


std::pair<int, int> GomukuAI::findBestMove(GomukuBoard &board) {
    int bestValue = INT_MIN;
    std::pair<int, int> bestMove = {-1, -1};

    auto possibleMoves = board.getPossibleMoves();

    for (auto &[x, y] : possibleMoves) {
        board.set(x, y, true);
        int moveValue = minimize(board, 1, INT_MIN, INT_MAX);
        std::cout << "Move: " << x << " " << y << " Value: " << moveValue << std::endl;
        board.reset(x, y);
        if (moveValue > bestValue) {
            bestMove = {x, y};
            bestValue = moveValue;
        }
    }
    return bestMove;
}

