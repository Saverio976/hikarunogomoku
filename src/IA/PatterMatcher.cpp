//
// Created by Théo on 10/11/2023.
//

#include "PatterMatcher.hpp"
#include "Bits.hpp"
#include "Perfcounter.hpp"
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>

bool PatternMatcher::isMatch(const Bits400& playerBoard, const Bits400& opponentBoard) const
{
    if (!_isValid) return false;
    auto counter = Perfcounter::Counter(Perfcounter::PerfType::BITSHIFT);
    bool playerContains = playerBoard.contains(_playerMaskPositionsCpy);
    bool opponentContains = opponentBoard.contains(_opponentMaskPositionsCpy);
    return playerContains && opponentContains;
}

bool PatternMatcher::set_increment(std::size_t x, std::size_t y)
{
    if (x >= 20 || y >= 20) {
        throw std::out_of_range("Position out of range");
    }
    if (x + _maxWidth >= 20 || y + _maxHeight >= 20) {
        _isValid = false;
        return false;
    }
    for (std::size_t i = 0; i < _playerMaskPositions.size(); ++i) {
        _playerMaskPositionsCpy[i] = _playerMaskPositions[i];
        _playerMaskPositionsCpy[i].x += x;
        _playerMaskPositionsCpy[i].y += y;
    }
    for (std::size_t i = 0; i < _opponentMaskPositions.size(); ++i) {
        _opponentMaskPositionsCpy[i] = _opponentMaskPositions[i];
        _opponentMaskPositionsCpy[i].x += x;
        _opponentMaskPositionsCpy[i].y += y;
    }
    _isValid = true;
    return true;
}
