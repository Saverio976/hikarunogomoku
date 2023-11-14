//
// Created by Théo on 14/11/2023.
//

#include "Bits.hpp"

Bits400::Bits400() : bits{} {}

Bits400::Bits400(const std::array<uint64_t, ARRAY_SIZE>& init_bits) : bits(init_bits) {}

Bits400::Bits400(const std::string& bitString) {
    if (bitString.length() != 400) {
        std::cout << "Invalid bit string length" << std::endl;
        return;
    }

    for (size_t i = 0; i < bitString.size(); ++i) {
        if (bitString[i] != '0' && bitString[i] != '1') {
            std::cout << "Invalid bit string" << std::endl;
            return;
        }

        size_t arrayIndex = i / 64;
        size_t bitIndex = i % 64;
        if (bitString[i] == '1') {
            bits[arrayIndex] |= (1ULL << bitIndex);
        }
    }
}

Bits400 Bits400::operator&(const Bits400& other) const {
    Bits400 result;
    for (int i = 0; i < ARRAY_SIZE - 1; ++i) {
        result.bits[i] = this->bits[i] & other.bits[i];
    }
    uint64_t lastElementMask = (1ULL << (400 % 64)) - 1;
    result.bits[ARRAY_SIZE - 1] = (this->bits[ARRAY_SIZE - 1] & other.bits[ARRAY_SIZE - 1]) & lastElementMask;

    return result;
}

Bits400 Bits400::operator|(const Bits400& other) const {
    Bits400 result;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        result.bits[i] = this->bits[i] | other.bits[i];
    }
    return result;
}

Bits400 Bits400::operator^(const Bits400& other) const {
    Bits400 result;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        result.bits[i] = this->bits[i] ^ other.bits[i];
    }
    return result;
}

Bits400 Bits400::operator>>(int shift) const {
    if (shift < 0) {
        throw std::invalid_argument("Shift must be non-negative.");
    }

    Bits400 result;
    int shiftWhole = shift / 64;
    int shiftPart = shift % 64;

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        result.bits[i] = (i + shiftWhole < ARRAY_SIZE) ? bits[i + shiftWhole] >> shiftPart : 0;

        if (shiftPart > 0 && i + shiftWhole + 1 < ARRAY_SIZE) {
            result.bits[i] |= bits[i + shiftWhole + 1] << (64 - shiftPart);
        }
    }

    return result;
}

bool Bits400::none() const {
    for (const auto& element : bits) {
        if (element != 0) {
            return false;
        }
    }
    return true;
}

void Bits400::set() {
    bits.fill(~0ULL);
}

void Bits400::set(size_t pos) {
    if (pos >= 400) {
        throw std::out_of_range("Position out of range");
    }
    size_t arrayIndex = pos / 64;
    size_t bitIndex = pos % 64;
    bits[arrayIndex] |= (1ULL << bitIndex);

}

void Bits400::reset() {
    bits.fill(0);
}

void Bits400::reset(size_t pos) {
    if (pos >= 400) {
        throw std::out_of_range("Position out of range");
    }
    size_t arrayIndex = pos / 64;
    size_t bitIndex = pos % 64;
    bits[arrayIndex] &= ~(1ULL << bitIndex);
}

bool Bits400::test(size_t pos) const {
    if (pos >= 400) {
        throw std::out_of_range("Position out of range");
    }
    size_t arrayIndex = pos / 64;
    size_t bitIndex = pos % 64;
    return (bits[arrayIndex] & (1ULL << bitIndex)) != 0;
}

Bits400& Bits400::operator=(const Bits400& other) {
    if (this != &other) {
        bits = other.bits;
    }
    return *this;
}

Bits400& Bits400::operator>>=(int shift) {
    *this = *this >> shift;
    return *this;
}

std::string Bits400::to_string() const {
    std::string str;
    str.reserve(400);

    for (int pos = 0; pos < 400; ++pos) {
        size_t arrayIndex = pos / 64;
        size_t bitIndex = pos % 64;
        str += (bits[arrayIndex] & (1ULL << bitIndex)) ? '1' : '0';
    }

    return str;
}