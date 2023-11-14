//
// Created by Théo on 10/11/2023.
//

#include "GomukuIA.hpp"
#include "Perfcounter.hpp"
#include <climits>
#include <random>

GomukuAI::GomukuAI(int depth) : maxDepth(depth) {
    auto patterns = Pattern20::getPatterns();
    for (auto& pattern : patterns) {
        auto matcher = PatternMatcher(pattern.getDataPlayer(), pattern.getDataOpponent(), pattern.getMask());
        patternMatchers.emplace_back(matcher, pattern.getScore());
    }
}

inline int GomukuAI::evaluateBoard(const GomukuBoard &board, bool isPayer) {
    int score = 0;
    Perfcounter::Counter counter(Perfcounter::PerfType::EVALUATE_BOARD);

    if (isPayer) {
        for (int i = 0; i < BOARD_BITS; ++i) {
            for (auto& patternMatcher : patternMatchers) {
                if (patternMatcher.first.isMatch(board.player, board.opponent)) {
                    score += patternMatcher.second;
                }
                patternMatcher.first.advance();
            }
        }
    } else {
        for (int i = 0; i < BOARD_BITS; ++i) {
            for (auto& patternMatcher : patternMatchers) {
                if (patternMatcher.first.isMatch(board.opponent, board.player)) {
                    score += patternMatcher.second;
                }
                patternMatcher.first.advance();
            }
        }
    }


    for (auto& patternMatcher : patternMatchers) {
        patternMatcher.first.reset();
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

