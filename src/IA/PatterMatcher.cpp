//
// Created by Théo on 10/11/2023.
//

#include "PatterMatcher.hpp"

PatternMatcher::PatternMatcher(std::bitset<BOARD_BITS> playerPattern, std::bitset<BOARD_BITS> opponentPattern,
                               std::bitset<BOARD_BITS> careMask) : playerPattern(playerPattern),
                                                                   opponentPattern(opponentPattern),
                                                                   careMask(careMask) {
    playerPatternCopy = playerPattern;
    opponentPatternCopy = opponentPattern;
    careMaskCopy = careMask;

    patterHeight = getPatternHeight();
    patternWidth = getPatternWidth();
}

bool PatternMatcher::advance() {
    playerPatternCopy >>= 1;
    opponentPatternCopy >>= 1;
    careMaskCopy >>= 1;

    if (++currentColumn >= BOARD_SIZE) {
        currentColumn = 0;
        ++currentRow;
    }

    bool isOutOfBounds = (currentRow + patterHeight > BOARD_SIZE) ||
                         (currentColumn + patternWidth > BOARD_SIZE);

    isValid = !isOutOfBounds;

    return isValid;
}

void PatternMatcher::reset() {
    playerPatternCopy = playerPattern;
    opponentPatternCopy = opponentPattern;
    careMaskCopy = careMask;
    isValid = true;
    currentColumn = 0;
    currentRow = 0;
}

bool PatternMatcher::isMatch(const std::bitset<BOARD_BITS>& playerBoard, const std::bitset<BOARD_BITS>& opponentBoard) const {
    if (!isValid) return false;
    std::bitset<BOARD_BITS> combinedMismatch = ((playerBoard ^ playerPatternCopy) | (opponentBoard ^ opponentPatternCopy)) & careMaskCopy;
    return combinedMismatch.none();
}

int PatternMatcher::getPatternHeight() const {
    int highestSetBit = -1;
    int lowestSetBit = BOARD_BITS;

    for (int i = 0; i < BOARD_BITS; ++i) {
        if (careMask.test(i)) {
            highestSetBit = std::max(highestSetBit, i);
            lowestSetBit = std::min(lowestSetBit, i);
        }
    }

    if (highestSetBit == -1) return 0;

    int highestRow = highestSetBit / BOARD_SIZE;
    int lowestRow = lowestSetBit / BOARD_SIZE;

    return (highestRow - lowestRow) + 1;
}

int PatternMatcher::getPatternWidth() const {
    int maxWidth = 0;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        int firstSetBit = -1;
        int lastSetBit = -1;

        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (careMask.test(row * BOARD_SIZE + col)) {
                if (firstSetBit == -1) firstSetBit = col;
                lastSetBit = col;
            }
        }

        if (firstSetBit == -1) continue;

        int width = (lastSetBit - firstSetBit) + 1;
        maxWidth = std::max(maxWidth, width);
    }
    return maxWidth;
}


