//
// Created by Théo on 10/11/2023.
//

#include "GomukuIA.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"

GomukuAI::GomukuAI(int depth) : maxDepth(depth) {

    scoreLookupTab[ScoreKey(5, 0)] = 100;   // 5 aligned stones with 0 open ends

    scoreLookupTab[ScoreKey(4, 0)] = 50;     // 4 aligned stones with 0 open ends

    scoreLookupTab[ScoreKey(3, 0)] = 20;     // 3 aligned stones with 0 open ends

    scoreLookupTab[ScoreKey(2, 0)] = 10;      // 2 aligned stones with 0 open ends

    scoreLookupTab[ScoreKey(1, 0)] = 1;       // 1 aligned stones with 0 open ends
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

    int potentialLineLength = 0;
    int stonesAligned = 0;
    bool spaceForWin = false;

    for (int i = 0; i < 5; ++i) {
        if (!isInBounds(x + i * dx, y + i * dy) || opponentBits.test(x + i * dx, y + i * dy)) {
            break;
        }
        if (bits.test(x + i * dx, y + i * dy)) {
            stonesAligned++;
        }
        potentialLineLength++;
    }

    for (int i = 1; i < 5; ++i) {
        if (!isInBounds(x - i * dx, y - i * dy) || opponentBits.test(x - i * dx, y - i * dy)) {
            break;
        }
        if (bits.test(x - i * dx, y - i * dy)) {
            stonesAligned++;
        }
        potentialLineLength++;
    }

    if (potentialLineLength >= 5) {
        spaceForWin = true;
    }

    if (spaceForWin) {
        switch (stonesAligned) {
            case 5: return scoreLookupTab[ScoreKey(5, 0)];
            case 4: return scoreLookupTab[ScoreKey(4, 0)];
            case 3: return scoreLookupTab[ScoreKey(3, 0)];
            case 2: return scoreLookupTab[ScoreKey(2, 0)];
            case 1: return scoreLookupTab[ScoreKey(1, 0)];
            default: break;
        }
    }

    return 0;
}

