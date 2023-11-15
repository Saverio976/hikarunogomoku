//
// Created by Theophilus Homawoo on 08/11/2023.
//

#include "GomukuBoard.hpp"

void GomukuBoard::set(int x, int y, bool isPlayer) {
    if (isPlayer) {
        player.set(x, y);
    } else {
        opponent.set(x, y);
    }
    updatePossibleMoves(x, y, true);
}

void GomukuBoard::reset(int x, int y) {
    player.reset(x, y);
    opponent.reset(x, y);
    updatePossibleMoves(x, y, false);
}

bool GomukuBoard::isOccupied(int x, int y) const {
    return player.test(x, y) || opponent.test(x, y);
}

std::vector<std::pair<int, int>> GomukuBoard::getPossibleMoves() const {
    // if (isFirstMove()) {
    //     return {std::make_pair(10, 10)};
    // }
    // return std::vector<std::pair<int, int>>(possibleMoves.begin(), possibleMoves.end());
    std::vector<std::pair<int, int>> moves;
    if (isFirstMove()) {
        moves.emplace_back(BOARD_SIZE / 2, BOARD_SIZE / 2);
        return moves;
    } else {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            for (int y = 0; y < BOARD_SIZE; ++y) {
                if (!isOccupied(x, y) && isAdjacentToOccupied(x, y)) {
                    moves.emplace_back(x, y);
                }
            }
        }
        return moves;
    }
}

bool GomukuBoard::isFirstMove() const {
    return player.none() && opponent.none();
}

void GomukuBoard::updatePossibleMoves(int x, int y, bool isSet) {
    // std::pair<int, int> move = std::make_pair(x, y);
    // if (isSet) {
    //     possibleMoves.erase(move);
    //     for (int dx = -1; dx <= 1; ++dx) {
    //         for (int dy = -1; dy <= 1; ++dy) {
    //             if (dx == 0 && dy == 0) continue;
    //             int newX = x + dx;
    //             int newY = y + dy;
    //             std::pair<int, int> adjMove = std::make_pair(newX, newY);
    //             if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
    //                 if (!isOccupied(newX, newY)) {
    //                     possibleMoves.insert(adjMove);
    //                     referenceCounts[adjMove]++;
    //                 }
    //             }
    //         }
    //     }
    // } else {
    //     for (int dx = -1; dx <= 1; ++dx) {
    //         for (int dy = -1; dy <= 1; ++dy) {
    //             if (dx == 0 && dy == 0) continue;
    //             int newX = x + dx;
    //             int newY = y + dy;
    //             std::pair<int, int> adjMove = std::make_pair(newX, newY);
    //             if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
    //                 referenceCounts[adjMove]--;
    //                 if (referenceCounts[adjMove] <= 0) {
    //                     possibleMoves.erase(adjMove);
    //                     referenceCounts.erase(adjMove);
    //                 }
    //             }
    //         }
    //     }
    //     if (isAdjacentToOccupied(x, y)) {
    //         possibleMoves.insert(move);
    //         referenceCounts[move] = 0;
    //     }
    // }
}

bool GomukuBoard::isAdjacentToOccupied(int x, int y) const {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int newX = x + dx;
            int newY = y + dy;
            if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
                if (isOccupied(newX, newY)) {
                    return true;
                }
            }
        }
    }
    return false;
}
