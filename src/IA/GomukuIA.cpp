//
// Created by Théo on 10/11/2023.
//

#include "GomukuIA.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"

GomukuAI::GomukuAI(int depth) : maxDepth(depth) {
    scoreLookupTab[ScoreKey(5, 2, 0)] = 10000000; // 5 in a row with 2 open ends and 0 gaps is a win.
    scoreLookupTab[ScoreKey(5, 1, 0)] = 10000000; // 5 in a row with 1 open end and 0 gaps is a win.
    scoreLookupTab[ScoreKey(5, 0, 0)] = 10000000; // 5 in a row with 0 open ends and 0 gaps is a win.

    scoreLookupTab[ScoreKey(4, 2, 0)] = 90000000; // 4 in a row with 2 open ends and 0 gaps is very strong.
    scoreLookupTab[ScoreKey(4, 1, 0)] = 80000000; // 4 in a row with 1 open end and 0 gaps is strong.
    scoreLookupTab[ScoreKey(4, 0, 0)] = 200000;   // 4 in a row with 0 open ends and 0 gaps is weaker.
    scoreLookupTab[ScoreKey(4, 1, 1)] = 100000;   // 4 in a row with 1 open end and 1 gap is less strong.
    scoreLookupTab[ScoreKey(4, 0, 1)] = 50000;    // 4 in a row with 0 open ends and 1 gap is even weaker.

    scoreLookupTab[ScoreKey(3, 2, 0)] = 500000;   // 3 in a row with 2 open ends and 0 gaps.
    scoreLookupTab[ScoreKey(3, 1, 0)] = 400000;   // 3 in a row with 1 open end and 0 gaps.
    scoreLookupTab[ScoreKey(3, 0, 0)] = 100000;   // 3 in a row with 0 open ends and 0 gaps.
    scoreLookupTab[ScoreKey(3, 2, 1)] = 50000;    // 3 in a row with 2 open ends and 1 gap.
    scoreLookupTab[ScoreKey(3, 1, 1)] = 10000;    // 3 in a row with 1 open end and 1 gap.

    scoreLookupTab[ScoreKey(2, 2, 0)] = 100000;   // 2 in a row with 2 open ends and 0 gaps.
    scoreLookupTab[ScoreKey(2, 1, 0)] = 50000;    // 2 in a row with 1 open end and 0 gaps.
    scoreLookupTab[ScoreKey(2, 0, 0)] = 10000;    // 2 in a row with 0 open ends and 0 gaps.
    scoreLookupTab[ScoreKey(2, 2, 1)] = 5000;     // 2 in a row with 2 open ends and 1 gap.
    scoreLookupTab[ScoreKey(2, 1, 1)] = 2500;     // 2 in a row with 1 open end and 1 gap.
}


inline int GomukuAI::evaluateBoard(const GomukuBoard &board) {
    int score = 0;
    Perfcounter::Counter counter(Perfcounter::PerfType::EVALUATE_BOARD);

    int lowerX = std::max(0, board.getMinX() - 1);
    int lowerY = std::max(0, board.getMinY() - 1);
    int upperX = std::min(BOARD_SIZE - 1, board.getMaxX() + 1);
    int upperY = std::min(BOARD_SIZE - 1, board.getMaxY() + 1);
    for (std::size_t y = lowerY; y <= upperY; ++y) {
        for (std::size_t x = lowerX; x <= upperX; ++x) {
            if (!board.isOccupied(x, y)) {
                continue;
            }
            score += evaluateDirection(board, x, y, 1, 0, true);
            score += evaluateDirection(board, x, y, 0, 1, true);
            score += evaluateDirection(board, x, y, 1, 1, true);
            score += evaluateDirection(board, x, y, 1, -1, true);

            score -= evaluateDirection(board, x, y, 1, 0, false) * 2;
            score -= evaluateDirection(board, x, y, 0, 1, false) * 2;
            score -= evaluateDirection(board, x, y, 1, 1, false) * 2;
            score -= evaluateDirection(board, x, y, 1, -1, false) * 2;
        }
    }
    return score;
}

std::pair<int, int> GomukuAI::findBestMove(GomukuBoard &board) {
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    auto moves = board.getPossibleMoves();

    if (moves.size() == 1) {
        return moves[0];
    }

    int depth = moves.size() > 15 ? 4 : 5;
    depth = moves.size() > 30 ? 3 : depth;

    int int_min = std::numeric_limits<int>::min();
    int int_max = std::numeric_limits<int>::max();

    for (const auto &move : board.getPossibleMoves()) {
        board.set(move.first, move.second, true);
        int moveScore = minValue(board, depth - 1, int_min, int_max);
        board.reset(move.first, move.second);

        if (moveScore > bestScore) {
            bestScore = moveScore;
            bestMove = move;
        }
    }

    return bestMove;
}

int GomukuAI::minimax(GomukuBoard &board, int depth, int alpha, int beta, bool isMaximizingPlayer) {
    if (depth == 0 || board.isGameOver()) {
        return evaluateBoard(board);
    }

    if (isMaximizingPlayer) {
        return maxValue(board, depth, alpha, beta);
    } else {
        return minValue(board, depth, alpha, beta);
    }
}

int GomukuAI::maxValue(GomukuBoard &board, int depth, int alpha, int beta) {
    if (depth == 0 || board.isGameOver()) {
        return evaluateBoard(board);
    }
    int value = std::numeric_limits<int>::min();

    for (auto move : board.getPossibleMoves()) {
        board.set(move.first, move.second, true);
        value = std::max(value, minValue(board, depth - 1, alpha, beta));
        board.reset(move.first, move.second);
        if (value >= beta) {
            return value;
        }
        alpha = std::max(alpha, value);
    }
    return value;
}

int GomukuAI::minValue(GomukuBoard &board, int depth, int alpha, int beta) {
    if (depth == 0 || board.isGameOver()) {
        return evaluateBoard(board);
    }
    int value = std::numeric_limits<int>::max();

    for (auto move : board.getPossibleMoves()) {
        board.set(move.first, move.second, false);
        value = std::min(value, maxValue(board, depth - 1, alpha, beta));
        board.reset(move.first, move.second);
        if (value <= alpha) {
            return value;
        }
        beta = std::min(beta, value);
    }
    return value;
}

bool GomukuAI::isInBounds(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

int GomukuAI::evaluateDirection(GomukuBoard board, int x, int y, int dx, int dy, bool isPlayer) {
    Bits400& bits = isPlayer ? board.player : board.opponent;
    Bits400& opponentBits = isPlayer ? board.opponent : board.player;

    int alignedStones = 0;
    int openEnds = 0;
    bool openStart = false, openEnd = false;
    int gaps = 0;

    if (isInBounds(x - dx, y - dy) && !opponentBits.test(x - dx, y - dy)) {
        openStart = true;
    }

    while (isInBounds(x, y)) {
        if (bits.test(x, y)) {
            alignedStones++;
        } else if (!opponentBits.test(x, y) && gaps == 0) {
            gaps++;
            alignedStones++;
        } else {
            break;
        }
        x += dx;
        y += dy;
    }

    if (isInBounds(x, y) && !opponentBits.test(x, y)) {
        openEnd = true;
    }

    if (openStart) openEnds++;
    if (openEnd) openEnds++;

    return scoreLookupTab[ScoreKey(alignedStones, openEnds, gaps)];
}
