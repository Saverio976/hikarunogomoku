//
// Created by Theophilus Homawoo on 08/11/2023.
//

#pragma once

#include "Bits.hpp"
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <random>

constexpr int BOARD_SIZE = 20;
constexpr int BOARD_BITS = BOARD_SIZE * BOARD_SIZE;

class GomukuBoard {
public:
    GomukuBoard();

    Bits400 player;
    Bits400 opponent;

    void set(int x, int y, bool isPlayer);
    void reset(int x, int y);
    bool isOccupied(int x, int y) const;
    std::vector<std::pair<int, int>> getPossibleMoves() const;

    bool isGameOver() const;

    int getMinX() const;
    int getMaxX() const;
    int getMinY() const;
    int getMaxY() const;

    bool hasFiveInARow(const Bits400 &bits) const;
    bool hasFiveInARow(const Bits400 &bits, const Bits400 &bits2) const;

    uint64_t computeHash() const;

private:
    bool isFirstMove() const;
    bool isAdjacentToOccupied(int x, int y) const;

    struct pair_hash {
        inline std::size_t operator()(const std::pair<int, int> &v) const {
            return v.first * 31 + v.second;
        }
    };

    std::unordered_set<std::pair<int, int>, pair_hash> possibleMoves;
    std::unordered_map<std::pair<int, int>, int, pair_hash> referenceCounts;

    void recalculateCorners();

    bool checkDirection(const Bits400 &bits, int x, int y, int dx, int dy) const;
    bool checkDirection(const Bits400 &bits, const Bits400 &bits2, int x, int y, int dx, int dy) const;

    bool isInBounds(int x, int y) const;

    void initializeZobristTable();

    std::array<std::array<uint64_t, BOARD_SIZE>, BOARD_SIZE> _playerZobristTable;
    std::array<std::array<uint64_t, BOARD_SIZE>, BOARD_SIZE> _opponentZobristTable;

    int _minX;
    int _minY;
    int _maxX;
    int _maxY;

};
