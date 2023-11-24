//
// Created by Théo on 10/11/2023.
//

#include "GomukuIA.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"

GomukuAI::GomukuAI(int depth) : maxDepth(depth), bestScore(std::numeric_limits<int>::min()){
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
    timeOut = false;
    std::vector<std::thread> threads;
    auto moves = board.getPossibleMoves();
    bestScore.store(std::numeric_limits<int>::min());
    bestMove = {-1, -1};

    int depth = moves.size() > 30 ? 3 : 4;

    const size_t num_threads = std::thread::hardware_concurrency();
    size_t moves_per_thread = moves.size() / num_threads;

    auto start_time = std::chrono::high_resolution_clock::now();
    std::atomic<size_t> active_threads(num_threads);
    std::condition_variable threads_done_cv;
    std::mutex threads_done_mutex;

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * moves_per_thread;
        size_t end = (i == num_threads - 1) ? moves.size() : (i + 1) * moves_per_thread;
        GomukuBoard boardCopy = board;
        threads.emplace_back([this, &boardCopy, moves, start, end, depth, &active_threads, &threads_done_cv, &threads_done_mutex]() {
            evaluateMoves(boardCopy, moves, start, end, depth);
            --active_threads;
            if (active_threads == 0) {
                std::lock_guard<std::mutex> lock(threads_done_mutex);
                threads_done_cv.notify_one();
            }
        });
    }

    std::thread timer_thread([&]() {
        std::unique_lock<std::mutex> lock(threads_done_mutex);
        if (!threads_done_cv.wait_until(lock, start_time + std::chrono::milliseconds(4980), [&] { return timeOut || active_threads == 0; })) {
            std::lock_guard<std::mutex> guard(bestMoveMutex);
            timeOut = true;
            cv.notify_all();
        }
    });

    for (auto& thread : threads) {
        thread.join();
    }

    if (timer_thread.joinable()) {
        timer_thread.join();
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
            case 5: return 100;
            case 4: return 50;
            case 3: return 20;
            case 2: return 10;
            case 1: return 1;
            default: break;
        }
    }

    return 0;
}

bool GomukuAI::didMoveBlockFour(GomukuBoard &board, int x, int y) {
    board.set(x, y, false);

    if (evaluateDirection(board, x, y, 1, 0, false) >= 100) {
        board.set(x, y, true);
        return true;
    } else if (evaluateDirection(board, x, y, 0, 1, false) >= 100) {
        board.set(x, y, true);
        return true;
    } else if (evaluateDirection(board, x, y, 1, 1, false) >= 100) {
        board.set(x, y, true);
        return true;
    } else if (evaluateDirection(board, x, y, 1, -1, false) >= 100) {
        board.set(x, y, true);
        return true;
    }
    return false;
}

void GomukuAI::evaluateMoves(GomukuBoard &board, const std::vector<std::pair<int, int>> &moves, size_t start,
                             size_t end, int depth) {
    int localBestScore = std::numeric_limits<int>::min();
    std::pair<int, int> localBestMove = {-1, -1};

    for (size_t i = start; i < end; ++i) {
        {
            std::lock_guard<std::mutex> guard(bestMoveMutex);
            if (timeOut) break;
        }

        const auto& move = moves[i];
        board.set(move.first, move.second, true);

        if (board.hasFiveInARow(board.player)) {
            {
                std::lock_guard<std::mutex> guard(bestMoveMutex);
                bestScore.store(std::numeric_limits<int>::max());
                bestMove = move;
                timeOut = true;
                cv.notify_all();
            }
            board.reset(move.first, move.second);
            return;
        }
        if (didMoveBlockFour(board, move.first, move.second)) {
            {
                std::lock_guard<std::mutex> guard(bestMoveMutex);
                bestScore.store(std::numeric_limits<int>::max());
                bestMove = move;
                timeOut = true;
                cv.notify_all();
            }
            board.reset(move.first, move.second);
            return;
        }

        int moveScore = minValue(board, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        board.reset(move.first, move.second);

        if (moveScore > localBestScore) {
            localBestScore = moveScore;
            localBestMove = move;
        }
    }

    {
        std::lock_guard<std::mutex> guard(bestMoveMutex);
        if (localBestScore > bestScore.load() && !timeOut) {
            bestScore.store(localBestScore);
            bestMove = localBestMove;
        }
    }
}

