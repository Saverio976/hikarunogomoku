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
    if (_minX > x) _minX = x;
    if (_maxX < x) _maxX = x;
    if (_minY > y) _minY = y;
    if (_maxY < y) _maxY = y;
}

void GomukuBoard::reset(int x, int y) {
    player.reset(x, y);
    opponent.reset(x, y);
    if (x == _minX || x == _maxX || y == _minY || y == _maxY) {
        recalculateCorners();
    }
}

void GomukuBoard::recalculateCorners() {
    for (int x = _minX; x <= _maxX; ++x) {
        for (int y = _minY; y <= _maxY; ++y) {
            if (isOccupied(x, y)) {
                if (_minX > x) _minX = x;
                if (_maxX < x) _maxX = x;
                if (_minY > y) _minY = y;
                if (_maxY < y) _maxY = y;
            }
        }
    }
}

bool GomukuBoard::isOccupied(int x, int y) const {
    return player.test(x, y) || opponent.test(x, y);
}

std::vector<std::pair<int, int>> GomukuBoard::getPossibleMoves() const {
    std::vector<std::pair<int, int>> moves;
    int _maxXP = std::min(_maxX + 1, BOARD_SIZE - 1);
    int _maxYP = std::min(_maxY + 1, BOARD_SIZE - 1);
    if (isFirstMove()) {
        moves.emplace_back(10, 10);
        return moves;
    } else {
        for (int x = _minX; x <= _maxXP; ++x) {
            for (int y = _minY; y <= _maxYP; ++y) {
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

bool GomukuBoard::isGameOver() const {
    if (hasFiveInARow(player) || hasFiveInARow(opponent)) {
        return true;
    }

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            if (!isOccupied(x, y)) {
                return false;
            }
        }
    }
    return true;
}

bool GomukuBoard::hasFiveInARow(const Bits400& bits) const {
    for (int x = _minX; x <= _maxX; ++x) {
        for (int y = _minY; y <= _maxY; ++y) {
            if (checkDirection(bits, x, y, 1, 0) ||
                checkDirection(bits, x, y, 0, 1) ||
                checkDirection(bits, x, y, 1, 1) ||
                checkDirection(bits, x, y, 1, -1)) {
                return true;
            }
        }
    }
    return false;
}

bool GomukuBoard::checkDirection(const Bits400& bits, int x, int y, int dx, int dy) const {
    int count = 0;

    while (isInBounds(x, y) && bits.test(x, y) && count < 5) {
        count++;
        x += dx;
        y += dy;
    }

    return count == 5;
}


bool GomukuBoard::isInBounds(int x, int y) const {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
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
