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
    // if (isFirstMove()) {
    //     return {std::make_pair(10, 10)};
    // }
    // return std::vector<std::pair<int, int>>(possibleMoves.begin(), possibleMoves.end());
    std::vector<std::pair<int, int>> moves;
    if (isFirstMove()) {
        moves.emplace_back(10, 10);
        return moves;
    } else {
        for (int x = _minX; x <= _maxX; ++x) {
            for (int y = _minY; y <= _maxY; ++y) {
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

int GomukuBoard::getMinX() const {
    return _minX;
}

int GomukuBoard::getMaxX() const {
    return _maxX;
}

int GomukuBoard::getMinY() const {
    return _minY;
}

int GomukuBoard::getMaxY() const {
    return _maxY;
}
