//
// Created by Théo on 14/11/2023.
//

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <iostream>

class Bits400 {
private:
    static const int ARRAY_SIZE = 7;
    std::array<uint64_t, ARRAY_SIZE> bits;

public:
    Bits400();
    Bits400(const std::array<uint64_t, ARRAY_SIZE>& init_bits);
    Bits400(const std::string& bitString);

    Bits400 operator&(const Bits400& other) const;
    Bits400 operator|(const Bits400& other) const;
    Bits400 operator^(const Bits400& other) const;
    Bits400 operator>>(int shift) const;
    Bits400& operator>>=(int shift);

    Bits400& operator=(const Bits400& other);

    bool none() const;
    void set();
    void set(size_t pos);
    void reset();
    void reset(size_t pos);
    bool test(size_t pos) const;
    std::string to_string() const;
};


