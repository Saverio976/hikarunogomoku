//
// Created by Théo on 14/11/2023.
//

#include "Bits.hpp"
#include <cstddef>
#include <vector>

bool Bits400::none() const {
    for (const auto& element : _bits) {
        for (const auto& bit : element) {
            if (bit) {
                return false;
            }
        }
    }
    return true;
}

void Bits400::set(std::size_t x, std::size_t y) {
    if (x >= 20 || y >= 20) {
        throw std::out_of_range("Position out of range");
    }
    _bits[y][x] = true;
}

void Bits400::reset(std::size_t x, std::size_t y)
{
    if (x >= 20 || y >= 20) {
        throw std::out_of_range("Position out of range");
    }
    _bits[y][x] = false;
}

bool Bits400::test(std::size_t x, std::size_t y) const {
    if (x >= 20 || y >= 20) {
        throw std::out_of_range("Position out of range");
    }
    return (_bits[y][x]);
}

Bits400& Bits400::operator=(const Bits400& other) {
    if (this != &other) {
        for (size_t i = 0; i < BOARD_SIZE; i++) {
            for (size_t j = 0; j < BOARD_SIZE; j++) {
                _bits[i][j] = other._bits[i][j];
            }
        }
    }
    return *this;
}

std::string Bits400::to_string() const
{
    std::string str;
    str.reserve(400);

    for (std::size_t y = 0; y < BOARD_SIZE; y++) {
        for (std::size_t x = 0; x < BOARD_SIZE; x++) {
            if (_bits[y][x]) {
                str += '1';
            } else {
                str += '0';
            }
        }
    }
    return str;
}

bool Bits400::contains(const std::vector<Position> &poss) const
{
   for (const auto& pos : poss) {
        if (_bits[pos.y][pos.x] != pos.value) {
            return false;
        }
    }
    return true;
}

void Bits400::print() const {
    for (std::size_t y = 0; y < BOARD_SIZE; y++) {
        for (std::size_t x = 0; x < BOARD_SIZE; x++) {
            if (_bits[y][x]) {
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << std::endl;
    }
}
