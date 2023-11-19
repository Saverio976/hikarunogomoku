//
// Created by Théo on 14/11/2023.
//

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

struct Position {
    Position(std::size_t x, std::size_t y, bool value) : x(x), y(y), value(value) {}
    std::size_t x;
    std::size_t y;
    bool value;
};

class Bits400 {
private:
    static const int BOARD_SIZE = 20;
    std::array<std::array<bool, BOARD_SIZE>, BOARD_SIZE> _bits;

public:
    constexpr Bits400()
    {
        this->reset();
    }
    constexpr Bits400(const std::string& bitString)
    {
        if (bitString.length() != 400) {
            std::cout << "Invalid bit string length" << std::endl;
            return;
        }

        for (size_t i = 0; i < bitString.size(); ++i) {
            if (bitString[i] != '0' && bitString[i] != '1') {
                std::cout << "Invalid bit string" << std::endl;
                return;
            }

            if (bitString[i] == '1') {
                std::size_t x = i / 20;
                std::size_t y = i % 20;
                _bits[y][x] = true;
            }
        }
    }

    bool contains(const std::vector<Position> &poss) const;

    Bits400& operator=(const Bits400& other);
    constexpr std::vector<Position> operator&(const Bits400& other)
    {
        std::vector<Position> positions;

        for (std::size_t y = 0; y < BOARD_SIZE; ++y) {
            for (std::size_t x = 0; x < BOARD_SIZE; ++x) {
                if (other._bits[y][x]) {
                    positions.push_back(Position(x, y, _bits[y][x]));
                }
            }
        }
        return positions;
    }

    bool none() const;
    void set(std::size_t x, std::size_t y);
    constexpr void reset()
    {
        for (auto& element : _bits) {
            element.fill(false);
        }
    }
    void reset(std::size_t x, std::size_t y);
    bool test(std::size_t x, std::size_t y) const;
    std::string to_string() const;
    void print() const;
};
