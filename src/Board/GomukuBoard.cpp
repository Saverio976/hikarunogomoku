//
// Created by Theophilus Homawoo on 08/11/2023.
//

#include "GomukuBoard.hpp"

void GomukuBoard::set(int x, int y, bool isPlayer) {
    int index = y * BOARD_SIZE + x;
    if (isPlayer) {
        player.set(index);
    } else {
        opponent.set(index);
    }
    updatePossibleMoves(x, y, true);
}

void GomukuBoard::reset(int x, int y) {
    int index = y * BOARD_SIZE + x;
    player.reset(index);
    opponent.reset(index);
    updatePossibleMoves(x, y, false);
}

bool GomukuBoard::isOccupied(int x, int y) const {
    int index = y * BOARD_SIZE + x;
    return player.test(index) || opponent.test(index);
}

std::vector<std::pair<int, int>> GomukuBoard::getPossibleMoves() const {
    if (isFirstMove()) {
        return {std::make_pair(10, 10)};
    }
    return std::vector<std::pair<int, int>>(possibleMoves.begin(), possibleMoves.end());
}

bool GomukuBoard::isFirstMove() const {
    return player.none() && opponent.none();
}

void GomukuBoard::updatePossibleMoves(int x, int y, bool isSet) {
    std::pair<int, int> move = std::make_pair(x, y);
    if (isSet) {
        possibleMoves.erase(move);
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int newX = x + dx;
                int newY = y + dy;
                std::pair<int, int> adjMove = std::make_pair(newX, newY);
                if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
                    if (!isOccupied(newX, newY)) {
                        possibleMoves.insert(adjMove);
                        referenceCounts[adjMove]++;
                    }
                }
            }
        }
    } else {
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int newX = x + dx;
                int newY = y + dy;
                std::pair<int, int> adjMove = std::make_pair(newX, newY);
                if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
                    referenceCounts[adjMove]--;
                    if (referenceCounts[adjMove] <= 0) {
                        possibleMoves.erase(adjMove);
                        referenceCounts.erase(adjMove);
                    }
                }
            }
        }
        if (isAdjacentToOccupied(x, y)) {
            possibleMoves.insert(move);
            referenceCounts[move] = 0;
        }
    }
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
