//
// Created by Théo on 10/11/2023.
//

#include <vector>
#include <thread>
#include "GomukuIA.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"

GomukuAI::GomukuAI(int depth) : _maxDepth(depth)
{

    _scoreLookupTab[ScoreKey(5, 0)] = 100;

    _scoreLookupTab[ScoreKey(4, 0)] = 50;

    _scoreLookupTab[ScoreKey(3, 0)] = 20;

    _scoreLookupTab[ScoreKey(2, 0)] = 10;

    _scoreLookupTab[ScoreKey(1, 0)] = 1;
}

inline int GomukuAI::evaluateBoard(const GomukuBoard &board)
{
    /**
    {
        std::lock_guard<std::mutex> lock(transpositionTableMutex);
        if (transpositionTable.find(board.currentHash) != transpositionTable.end()) {
            return transpositionTable[board.currentHash];
        }
    }
    **/
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
    /**
    std::lock_guard<std::mutex> lock(transpositionTableMutex);
    transpositionTable[board.currentHash] = score;
    **/
    return score;
}

std::pair<int, std::pair<int, int>> GomukuAI::findBestMoveThread(GomukuBoard &board, int depth, const std::vector<std::pair<int, int>> &moves)
{
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};
    int int_min = std::numeric_limits<int>::min();
    int int_max = std::numeric_limits<int>::max();

    for (const auto &move : moves) {
        board.set(move.first, move.second, true);
        if (board.hasFiveInARow(board.player)) {
            board.reset(move.first, move.second);
            bestScore = int_max;
            bestMove = move;
            continue;
        } else if ([&]() {
            board.set(move.first, move.second, false);
            bool hasFive = board.hasFiveInARow(board.opponent);
            board.set(move.first, move.second, true);
            return hasFive;
        }()) {
            board.reset(move.first, move.second);
            if (bestScore < int_max - 2000) {
                bestScore = int_max - 2000;
                bestMove = move;
            }
            continue;
        }
        int moveScore = minValue(board, depth - 1, int_min, int_max);
        board.reset(move.first, move.second);

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

    std::thread timerThread([this]() {
        std::unique_lock<std::mutex> lk(cv_m);
        if(cv.wait_for(lk, std::chrono::milliseconds(4900)) == std::cv_status::timeout) {
            this->searchInterrupted.store(true);
        }
    });

    int depth = moves.size() > 30 ? 3 : 4;

    for (std::size_t i = 0; i < nb_thread; ++i) {
        std::size_t start_i = i * slice_number;
        std::size_t end_i = (i + 1) * slice_number;
        auto slice_start = (i == 0) ? moves.begin() : moves.begin() + (i * slice_number);
        auto slice_end = (i == nb_thread - 1) ? moves.end() : moves.begin() + ((i + 1) * slice_number);
        std::vector<std::pair<int, int>> movesThread(slice_start, slice_end);
        threads.push_back(
            std::thread(
                [this, &board, depth, &mutexBest, &bestScore, &bestMove, movesThread]() {
                    auto boardThread = board;
                    auto best = findBestMoveThread(
                        boardThread,
                        depth,
                        movesThread
                    );
                    mutexBest.lock();
                    if (best.first > bestScore && !this->searchInterrupted.load()) {
                        bestScore = best.first;
                        bestMove = best.second;
                    }
                    mutexBest.unlock();
                }
            )
        );
    }

    for (auto &thread : threads) {
        thread.join();
    }

    {
        std::lock_guard<std::mutex> lk(cv_m);
        cv.notify_one();
    }

    if (timerThread.joinable()) {
        timerThread.join();
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
            case 5: return _scoreLookupTab[ScoreKey(5, 0)];
            case 4: return _scoreLookupTab[ScoreKey(4, 0)];
            case 3: return _scoreLookupTab[ScoreKey(3, 0)];
            case 2: return _scoreLookupTab[ScoreKey(2, 0)];
            case 1: return _scoreLookupTab[ScoreKey(1, 0)];
            default: break;
        }
    }

    return 0;
}

