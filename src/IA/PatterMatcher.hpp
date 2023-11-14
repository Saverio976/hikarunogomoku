//
// Created by Théo on 10/11/2023.
//

#pragma once

#include <bitset>
#include <fstream>
#include <string>
#include <iostream>
#include "GomukuBoard.hpp"

class PatternMatcher {
public:
    PatternMatcher(std::bitset<BOARD_BITS> playerPattern, std::bitset<BOARD_BITS> opponentPattern, std::bitset<BOARD_BITS> careMask);
    bool advance();
    void reset();
    bool isMatch(const std::bitset<BOARD_BITS>& playerBoard, const std::bitset<BOARD_BITS>& opponentBoard) const;
private:
    int patterHeight = 0;
    int patternWidth = 0;
    int shiftIndex = 0;
    int currentRow = 0;
    int currentColumn = 0;
    bool isValid = true;
    int getPatternHeight() const;
    int getPatternWidth() const;

    // Patterns
    std::bitset<BOARD_BITS> playerPattern;
    std::bitset<BOARD_BITS> opponentPattern;
    std::bitset<BOARD_BITS> careMask;

    std::bitset<BOARD_BITS> playerPatternCopy;
    std::bitset<BOARD_BITS> opponentPatternCopy;
    std::bitset<BOARD_BITS> careMaskCopy;
};