//
// Created by Théo on 10/11/2023.
//

#pragma once

#include "Bits.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include "GomukuBoard.hpp"

class PatternMatcher {
public:
    PatternMatcher(Bits400 playerPattern, Bits400 opponentPattern, Bits400 careMask);
    bool advance();
    void reset();
    bool isMatch(const Bits400& playerBoard, const Bits400& opponentBoard) const;
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
    Bits400 playerPattern;
    Bits400 opponentPattern;
    Bits400 careMask;

    Bits400 playerPatternCopy;
    Bits400 opponentPatternCopy;
    Bits400 careMaskCopy;
};