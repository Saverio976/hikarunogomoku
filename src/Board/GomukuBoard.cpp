//
// Created by Theophilus Homawoo on 08/11/2023.
//

#include "GomukuBoard.hpp"

void GomukuBoard::set(int x, int y, bool isPlayer) {
    int invertedIndex = (BOARD_SIZE - 1 - y) * BOARD_SIZE + (BOARD_SIZE - 1 - x);
    if (isPlayer) {
        player.set(invertedIndex);
    } else {
        opponent.set(invertedIndex);
    }
}

void GomukuBoard::reset(int x, int y) {
    int invertedIndex = (BOARD_SIZE - 1 - y) * BOARD_SIZE + (BOARD_SIZE - 1 - x);
    player.reset(invertedIndex);
    opponent.reset(invertedIndex);
}

bool GomukuBoard::isOccupied(int x, int y) const {
    int invertedIndex = (BOARD_SIZE - 1 - y) * BOARD_SIZE + (BOARD_SIZE - 1 - x);
    return player.test(invertedIndex) || opponent.test(invertedIndex);
}

std::vector<std::pair<int, int>> GomukuBoard::getPossibleMoves() const {
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