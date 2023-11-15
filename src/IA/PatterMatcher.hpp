//
// Created by Théo on 10/11/2023.
//

#pragma once

#include "Bits.hpp"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "GomukuBoard.hpp"

class PatternMatcher {
public:
    constexpr PatternMatcher(Bits400 playerPattern, Bits400 opponentPattern, Bits400 careMask)
    {
        _playerMaskPositions = playerPattern & careMask;
        _opponentMaskPositions = opponentPattern & careMask;
        _playerMaskPositionsCpy = _playerMaskPositions;
        _opponentMaskPositionsCpy = _opponentMaskPositions;
        const auto [minY, maxY] = std::minmax_element(_playerMaskPositions.begin(), _playerMaskPositions.end(), [](const Position& a, const Position& b) {
            return a.y < b.y;
        });
        const auto [minX, maxX] = std::minmax_element(_playerMaskPositions.begin(), _playerMaskPositions.end(), [](const Position& a, const Position& b) {
            return a.x < b.x;
        });
        _maxWidth = maxX->x - minX->x;
        _maxHeight = maxY->y - minY->y;
    }

    bool isMatch(const Bits400& playerBoard, const Bits400& opponentBoard) const;
    bool set_increment(std::size_t x, std::size_t y);

private:
    bool _isValid = true;
    // Patterns
    std::vector<Position> _playerMaskPositions;
    std::vector<Position> _playerMaskPositionsCpy;
    std::vector<Position> _opponentMaskPositions;
    std::vector<Position> _opponentMaskPositionsCpy;

    std::size_t _maxWidth = 0;
    std::size_t _maxHeight = 0;
};
