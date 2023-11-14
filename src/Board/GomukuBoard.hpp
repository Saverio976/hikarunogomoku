//
// Created by Theophilus Homawoo on 08/11/2023.
//

#pragma once

#include "Bits.hpp"
#include <array>
#include <vector>

constexpr int BOARD_SIZE = 20;
constexpr int BOARD_BITS = BOARD_SIZE * BOARD_SIZE;

class GomukuBoard {
public:
    Bits400 player;
    Bits400 opponent;

    void set(int x, int y, bool isPlayer);
    void reset(int x, int y);
    bool isOccupied(int x, int y) const;
    std::vector<std::pair<int, int>> getPossibleMoves() const;
private:
    bool isFirstMove() const;
    bool isAdjacentToOccupied(int x, int y) const;
};
