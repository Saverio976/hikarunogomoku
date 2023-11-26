//
// Created by Théo on 10/11/2023.
//

#include <vector>
#include <thread>
#include "GomukuIA.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"

GomukuAI::GomukuAI(int depth) : _maxDepth(depth), _pool(5)
{
    std::vector<int> win = {1, 1, 1, 1, 1};
    _movesPatterns.emplace_back(std::make_pair(100, win));
    std::vector<int> nextIsWin = {0, 1, 1, 1, 1, 0};
    _movesPatterns.emplace_back(std::make_pair(95, nextIsWin));

    std::vector<int> nextIsNotSureWin = {0, 1, 1, 1, 1};
    _movesPatterns.emplace_back(std::make_pair(80, nextIsNotSureWin));
    std::vector<int> nextIsNotSureWin2 = {1, 1, 1, 1, 0};
    _movesPatterns.emplace_back(std::make_pair(80, nextIsNotSureWin2));
    std::vector<int> nextIsNotSureWin3 = {1, 1, 1, 0, 1};
    _movesPatterns.emplace_back(std::make_pair(80, nextIsNotSureWin3));
    std::vector<int> nextIsNotSureWin4 = {1, 1, 0, 1, 1};
    _movesPatterns.emplace_back(std::make_pair(80, nextIsNotSureWin4));
    std::vector<int> nextIsNotSureWin5 = {1, 0, 1, 1, 1};
    _movesPatterns.emplace_back(std::make_pair(80, nextIsNotSureWin5));

    std::vector<int> nextIsNotWinButOk = {0, 0, 1, 1, 1, 0};
    _movesPatterns.emplace_back(std::make_pair(50, nextIsNotWinButOk));
    std::vector<int> nextIsNotWinButOk2 = {0, 1, 1, 1, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(50, nextIsNotWinButOk2));

    std::vector<int> nextIsNotWinButOk3 = {0, 0, 1, 1, 1};
    _movesPatterns.emplace_back(std::make_pair(40, nextIsNotWinButOk3));
    std::vector<int> nextIsNotWinButOk4 = {1, 1, 1, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(40, nextIsNotWinButOk4));

    std::vector<int> stillPlaceForWin = {0, 0, 1, 1, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(20, stillPlaceForWin));
    std::vector<int> stillPlaceForWinNButLess = {0, 1, 1, 0, 0};

    _movesPatterns.emplace_back(std::make_pair(15, stillPlaceForWinNButLess));
    std::vector<int> stillPlaceForWinNButLess2 = {0, 0, 1, 1, 0};
    _movesPatterns.emplace_back(std::make_pair(15, stillPlaceForWinNButLess2));

    std::vector<int> stillPlaceForWinNButLessLess = {1, 1, 0, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(15, stillPlaceForWinNButLessLess));
    std::vector<int> stillPlaceForWinNButLessLess2 = {0, 0, 0, 1, 1};
    _movesPatterns.emplace_back(std::make_pair(15, stillPlaceForWinNButLessLess2));

    std::vector<int> thisSuck = {1, 0, 0, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(1, thisSuck));
    std::vector<int> thisSuck2 = {0, 1, 0, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(1, thisSuck2));
    std::vector<int> thisSuck3 = {0, 0, 1, 0, 0};
    _movesPatterns.emplace_back(std::make_pair(1, thisSuck3));
    std::vector<int> thisSuck4 = {0, 0, 0, 1, 0};
    _movesPatterns.emplace_back(std::make_pair(1, thisSuck4));
    std::vector<int> thisSuck5 = {0, 0, 0, 0, 1};
    _movesPatterns.emplace_back(std::make_pair(1, thisSuck5));
}

inline int GomukuAI::evaluateBoard(const GomukuBoard &board)
{
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
            score += evaluateDirection(board, x, y, 1, 0);
            score += evaluateDirection(board, x, y, 0, 1);
            score += evaluateDirection(board, x, y, 1, 1);
            score += evaluateDirection(board, x, y, 1, -1);
        }
    }
    return score;
}

std::pair<int, std::pair<int, int>> GomukuAI::findBestMoveThread(GomukuBoard &board, int depth, const std::vector<std::pair<int, int>> &moves)
{
    GomukuBoard boardCopy = board;

    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};
    int int_min = std::numeric_limits<int>::min();
    int int_max = std::numeric_limits<int>::max();

    for (const auto &move : moves) {
        boardCopy.set(move.first, move.second, false);
        if (boardCopy.hasFiveInARow(boardCopy.opponent)) {
            bestScore = int_max - 100;
            bestMove = move;
            boardCopy.reset(move.first, move.second);
            continue;
        }
        boardCopy.reset(move.first, move.second);
        boardCopy.set(move.first, move.second, true);
        if (boardCopy.hasFiveInARow(board.player)) {
            boardCopy.reset(move.first, move.second);
            return {int_max, move};
        }
        int moveScore = minValue(board, depth - 1, int_min, int_max);
        boardCopy.reset(move.first, move.second);

        if (moveScore > bestScore) {
            bestScore = moveScore;
            bestMove = move;
        }
    }
    return {bestScore, bestMove};
}

std::pair<int, int> GomukuAI::findBestMove(GomukuBoard &board) {
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};

    auto moves = board.getPossibleMoves();

    if (moves.size() == 1) {
        return moves[0];
    }

    std::size_t nb_thread = 5;
    std::size_t slice_number = moves.size() / nb_thread;
    std::vector<std::thread> threads;
    std::mutex mutexBest;

    int depth = moves.size() > 30 ? 3 : 4;

    std::vector<std::future<std::pair<int, std::pair<int, int>>>> futures;

    for (std::size_t i = 0; i < nb_thread; ++i) {
        auto slice_start = (i == 0) ? moves.begin() : moves.begin() + (i * slice_number);
        auto slice_end = (i == nb_thread - 1) ? moves.end() : moves.begin() + ((i + 1) * slice_number);
        futures.emplace_back(
                _pool.enqueue([this, &board, depth, slice_start, slice_end]() {
                    return findBestMoveThread(board, depth, std::vector<std::pair<int, int>>(slice_start, slice_end));
                })
        );
    }

    for (auto &future : futures) {
        auto result = future.get();
        if (result.first > bestScore) {
            bestScore = result.first;
            bestMove = result.second;
        }
    }
    return bestMove;
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

int GomukuAI::evaluateDirection(GomukuBoard board, int x, int y, int dx, int dy) {
    Bits400& bits = board.player;
    Bits400& opponentBits = board.opponent;

    std::array<int, 9> tableNowPlayer = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<int, 9> tableMaxOpponent = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int valPlayer = 0;
    int valOpponent = 0;

    for (int i = -4; i < 5; i++) {
        if (!isInBounds(x + i * dx, y + i * dy)) {
            continue;
        }
        if (opponentBits.test(x + i * dx, y + i * dy)) {
            tableNowPlayer[i + 4] = 2;
            tableMaxOpponent[i + 4] = 1;
        } else if (bits.test(x + i * dx, y + i * dy)) {
            tableNowPlayer[i + 4] = 1;
            tableMaxOpponent[i + 4] = 2;
        }
    }
    for (int i = 0; i < 9; i++) {
        for (const auto &pattern : _movesPatterns) {
            if (i + pattern.second.size() >= 9) {
                continue;
            }
            bool matchPlayer = true;
            bool matchOpponent = true;
            int curIndex = i;
            for (const auto &value : pattern.second) {
                if (tableNowPlayer[curIndex] != value) {
                    matchPlayer = false;
                }
                if (tableMaxOpponent[curIndex] != value) {
                    matchOpponent = false;
                }
                if (!matchOpponent && !matchPlayer) {
                    break;
                }
                curIndex++;
            }
            if (matchOpponent) {
                valOpponent = std::max(valOpponent, pattern.first);
            }
            if (matchPlayer) {
                valPlayer = std::max(valPlayer, pattern.first);
            }
        }
    }
    return valPlayer - valOpponent;
}
