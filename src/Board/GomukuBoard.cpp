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

    updatePossibleMoves(x, y, true);
}

void GomukuBoard::reset(int x, int y) {
    int invertedIndex = (BOARD_SIZE - 1 - y) * BOARD_SIZE + (BOARD_SIZE - 1 - x);
    player.reset(invertedIndex);
    opponent.reset(invertedIndex);
    updatePossibleMoves(x, y, false);
}

bool GomukuBoard::isOccupied(int x, int y) const {
    int invertedIndex = (BOARD_SIZE - 1 - y) * BOARD_SIZE + (BOARD_SIZE - 1 - x);
    return player.test(invertedIndex) || opponent.test(invertedIndex);
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
    if (isSet) {
        possibleMoves.erase(std::make_pair(x, y));

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int newX = x + dx;
                int newY = y + dy;
                if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE && !isOccupied(newX, newY)) {
                    possibleMoves.insert(std::make_pair(newX, newY));
                }
            }
        }
    } else {
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int adjacentX = x + dx;
                int adjacentY = y + dy;
                if (adjacentX >= 0 && adjacentX < BOARD_SIZE && adjacentY >= 0 && adjacentY < BOARD_SIZE) {
                    if (isOccupied(adjacentX, adjacentY)) {
                        possibleMoves.insert(std::make_pair(x, y));
                        break;
                    }
                }
            }
        }
    }
}
