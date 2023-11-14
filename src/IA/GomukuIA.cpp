//
// Created by Théo on 10/11/2023.
//

#include "GomukuIA.hpp"
#include <climits>

std::bitset<BOARD_BITS> reverse(std::bitset<BOARD_BITS> bitset) {
    std::bitset<BOARD_BITS> reversed;

    for (int i = 0; i < BOARD_BITS; ++i) {
        reversed.set(i, bitset.test(BOARD_BITS - i - 1));
    }

    return reversed;
}

GomukuAI::GomukuAI(int depth) : maxDepth(depth) {
    std::bitset<BOARD_BITS> ThreeInHorizontalPattern;
    std::bitset<BOARD_BITS> ThreeInHorizontalopponentPattern;
    std::bitset<BOARD_BITS> ThreeInHorizontalcareMask;

    ThreeInHorizontalPattern.set(1);
    ThreeInHorizontalPattern.set(2);
    ThreeInHorizontalPattern.set(3);

    ThreeInHorizontalcareMask.set(1);
    ThreeInHorizontalcareMask.set(2);
    ThreeInHorizontalcareMask.set(3);

    ThreeInHorizontalPattern = reverse(ThreeInHorizontalPattern);
    ThreeInHorizontalcareMask = reverse(ThreeInHorizontalcareMask);

    PatternMatcher ThreeInHorizontalpatternMatcher(ThreeInHorizontalPattern, ThreeInHorizontalopponentPattern, ThreeInHorizontalcareMask);

    std::bitset<BOARD_BITS> ThreeInVerticalPattern;
    std::bitset<BOARD_BITS> ThreeInVerticalOpponentPattern;
    std::bitset<BOARD_BITS> ThreeInVerticalcareMask;

    ThreeInVerticalPattern.set(20);
    ThreeInVerticalPattern.set(40);
    ThreeInVerticalPattern.set(60);

    ThreeInVerticalcareMask.set(0);
    ThreeInVerticalcareMask.set(20);
    ThreeInVerticalcareMask.set(40);
    ThreeInVerticalcareMask.set(60);
    ThreeInVerticalcareMask.set(80);

    ThreeInVerticalPattern = reverse(ThreeInVerticalPattern);
    ThreeInVerticalOpponentPattern = reverse(ThreeInVerticalOpponentPattern);
    ThreeInVerticalcareMask = reverse(ThreeInVerticalcareMask);

    PatternMatcher ThreeInVerticalpatternMatcher(ThreeInVerticalPattern, ThreeInVerticalOpponentPattern, ThreeInVerticalcareMask);

    patternMatchers.emplace_back(ThreeInVerticalpatternMatcher, 100);
    patternMatchers.emplace_back(ThreeInHorizontalpatternMatcher, 100);
}

int GomukuAI::evaluateBoard(const GomukuBoard &board) {
    int score = 0;

    for (int i = 0; i < BOARD_BITS; ++i) {
        for (auto& patternMatcher : patternMatchers) {
            if (patternMatcher.first.isMatch(board.player, board.opponent)) {
                score += patternMatcher.second;
            }
            patternMatcher.first.advance();
        }
    }
    for (auto& patternMatcher : patternMatchers) {
        patternMatcher.first.reset();
    }
    return score;
}

int GomukuAI::maximize(GomukuBoard& board, int depth, int alpha, int beta) {
    if (depth == maxDepth) {
        return evaluateBoard(board);
    }

    int maxEval = INT_MIN;
    auto possibleMoves = board.getPossibleMoves();

    for (auto &[x, y] : possibleMoves) {
        if (!board.isOccupied(x, y)) {
            board.set(x, y, true);
            int eval = minimize(board, depth + 1, alpha, beta);
            board.reset(x, y);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                return maxEval;
            }
        }
    }
    return maxEval;
}

int GomukuAI::minimize(GomukuBoard& board, int depth, int alpha, int beta) {
    if (depth == maxDepth) {
        return evaluateBoard(board);
    }

    int minEval = INT_MAX;
    auto possibleMoves = board.getPossibleMoves();

    for (auto &[x, y] : possibleMoves) {
        if (!board.isOccupied(x, y)) {
            board.set(x, y, false);
            int eval = maximize(board, depth + 1, alpha, beta);
            board.reset(x, y);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                return minEval;
            }
        }
    }
    return minEval;
}


std::pair<int, int> GomukuAI::findBestMove(GomukuBoard &board) {
    int bestValue = true ? INT_MIN : INT_MAX;
    std::pair<int, int> bestMove = {-1, -1};

    auto possibleMoves = board.getPossibleMoves();


    for (auto &[x, y] : possibleMoves) {
        if (!board.isOccupied(x, y)) {
            board.set(x, y, true);
            int moveValue = maximize(board, 0, INT_MIN, INT_MAX);
            board.reset(x, y);
            if (moveValue > bestValue) {
                bestMove = {x, y};
                bestValue = moveValue;
            }
        }
    }
    return bestMove;
}

