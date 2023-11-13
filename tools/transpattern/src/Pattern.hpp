#pragma once

#include <bitset>
#include <cstddef>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ostream>
#include <filesystem>

#define GET_INDEX(d_board_size, d_x, d_y) (((d_board_size - 1 - y) * (d_board_size)) + (d_board_size - 1 - x))

template <std::size_t BOARD_SIZE>
class Pattern {
public:
    static Pattern fromFile(const std::string &patternFile)
    {
        Pattern pattern;
        std::string buffer;

        if (!std::filesystem::is_regular_file(patternFile)) {
            throw std::runtime_error("Pattern file not found ('" + patternFile + "')");
        }
        std::ifstream file(patternFile);
        if (!file.is_open() || !file.good() || file.bad()) {
            throw std::runtime_error("Failed to open pattern file ('" + patternFile + "')");
        }
        while (std::getline(file, buffer)) {
            if (buffer.empty() || buffer.starts_with("#")) {
                continue;
            }
            if (buffer == "desciption") {
                pattern.description = Pattern<BOARD_SIZE>::getDescription(file);
            } else if (buffer == "pattern") {
                auto [sizeX, sizeY] = Pattern<BOARD_SIZE>::getPattern(file);
                if (sizeX != BOARD_SIZE || sizeY != BOARD_SIZE) {
                    throw std::runtime_error("Invalid pattern size (" + std::to_string(sizeX) + "x" + std::to_string(sizeY) + ")");
                }
                pattern.sizeX = sizeX;
                pattern.sizeY = sizeY;
            } else if (buffer == "data") {
                auto [dataPlayer, dataOpponent] = Pattern<BOARD_SIZE>::getPatternData(file);
                pattern.dataPlayer = dataPlayer;
                pattern.dataOpponent = dataOpponent;
            } else if (buffer == "mask") {
                pattern.mask = Pattern<BOARD_SIZE>::getMask(file);
            } else if (buffer == "score") {
                pattern.score = getScore(file);
            }
        }
        return pattern;
    }

    static std::string definitionToString()
    {
        std::string def;

        def += "#pragma once\n";
        def += "\n";
        def += "#include <bitset>\n";
        def += "#include <string>\n";
        def += "#include <cstddef>\n";
        def += "#include <vector>\n";
        def += "\n";
        def += "class Pattern" + std::to_string(BOARD_SIZE) + " {\n";
        def += "public:\n";
        def += "    static std::vector<Pattern" + std::to_string(BOARD_SIZE) + "> getPatterns();\n";
        def += "\n";
        def += "    std::size_t getSizeX() const { return _sizeX; };\n";
        def += "    std::size_t getSizeY() const { return _sizeY; };\n";
        def += "    const std::bitset<" + std::to_string(BOARD_SIZE * BOARD_SIZE) + "> &getDataPlayer() const { return _dataPlayer; };\n";
        def += "    const std::bitset<" + std::to_string(BOARD_SIZE * BOARD_SIZE) + "> &getDataOpponent() const { return _dataOpponent; };\n";
        def += "    const std::bitset<" + std::to_string(BOARD_SIZE * BOARD_SIZE) + "> &getMask() const { return _mask; };\n";
        def += "    const std::string &getDescription() const { return _description; };\n";
        def += "    float getScore() const { return _score; };\n";
        def += "\n";
        def += "private:\n";
        def += "    Pattern" + std::to_string(BOARD_SIZE) + "(std::size_t sizeX, std::size_t sizeY, std::string bitDataPlayer, std::string bitDataOpponent, std::string bitMask, std::string description, float score);\n";
        def += "\n";
        def += "    std::size_t _sizeX;\n";
        def += "    std::size_t _sizeY;\n";
        def += "    std::bitset<" + std::to_string(BOARD_SIZE * BOARD_SIZE) + "> _dataPlayer;\n";
        def += "    std::bitset<" + std::to_string(BOARD_SIZE * BOARD_SIZE) + "> _dataOpponent;\n";
        def += "    std::bitset<" + std::to_string(BOARD_SIZE * BOARD_SIZE) + "> _mask;\n";
        def += "    std::string _description;\n";
        def += "    float _score;\n";
        def += "};\n";
        return def;
    }
    static std::string implementationToString(std::vector<Pattern> patterns)
    {
        std::string impl;

        impl += "#include \"Pattern" + std::to_string(BOARD_SIZE) + ".hpp\"\n";
        impl += "\n";
        impl += "std::vector<Pattern" + std::to_string(BOARD_SIZE) + "> Pattern" + std::to_string(BOARD_SIZE) + "::getPatterns()\n";
        impl += "{\n";
        impl += "    std::vector<Pattern" + std::to_string(BOARD_SIZE) + "> patterns = {\n";
        for (const auto &pattern : patterns) {
            impl += "        " + pattern.toString() + ",\n";
        }
        impl += "    };\n";
        impl += "    return patterns;\n";
        impl += "}\n";
        impl += "\n";
        impl += "Pattern" + std::to_string(BOARD_SIZE) + "::Pattern" + std::to_string(BOARD_SIZE) + "(std::size_t sizeX, std::size_t sizeY, std::string bitDataPlayer, std::string bitDataOpponent, std::string bitMask, std::string description, float score)\n";
        impl += "    : _sizeX(sizeX), _sizeY(sizeY), _dataPlayer(bitDataPlayer), _dataOpponent(bitDataOpponent), _mask(bitMask), _description(description), _score(score)\n";
        impl += "{\n";
        impl += "}\n";
        return impl;
    }

private:
    Pattern() = default;

    std::string toString() const
    {
        std::string str;

        str += "Pattern" + std::to_string(BOARD_SIZE) + "(";
        str += std::to_string(this->sizeX) + ", ";
        str += std::to_string(this->sizeY) + ", ";
        str += "\"" + this->dataPlayer.to_string() + "\", ";
        str += "\"" + this->dataOpponent.to_string() + "\", ";
        str += "\"" + this->mask.to_string() + "\", ";
        str += "\"" + this->description + "\", ";
        str += std::to_string(this->score) + ")";
        return str;
    }

    static std::string getDescription(std::ifstream &file)
    {
        std::string buffer;

        std::getline(file, buffer);
        return buffer;
    }

    static std::pair<std::size_t, std::size_t> getPattern(std::ifstream &file)
    {
        std::string buffer;
        std::getline(file, buffer);
        std::stringstream ss(buffer);
        std::size_t sizeX;
        std::size_t sizeY;
        ss >> sizeX >> sizeY;
        return std::make_pair(sizeX, sizeY);
    }

    static std::pair<std::bitset<BOARD_SIZE * BOARD_SIZE>, std::bitset<BOARD_SIZE * BOARD_SIZE>> getPatternData(std::ifstream &file)
    {
        std::bitset<BOARD_SIZE * BOARD_SIZE> dataPlayer;
        std::bitset<BOARD_SIZE * BOARD_SIZE> dataOpponent;

        for (std::size_t y = 0; y < BOARD_SIZE; y++) {
            std::string bufferLine;
            std::getline(file, bufferLine);
            std::stringstream ss(bufferLine);
            std::string buffer;
            std::size_t x = 0;

            while (std::getline(ss, buffer, ' ')) {
                char c;
                if (std::istringstream(buffer) >> c) {
                    if (c == 'P') {
                        dataPlayer.set(GET_INDEX(BOARD_SIZE, x, y));
                    } else if (c == 'O') {
                        dataOpponent.set(GET_INDEX(BOARD_SIZE, x, y));
                    }
                }
                x++;
            }
        }
        return std::make_pair(dataPlayer, dataOpponent);
    }

    static std::bitset<BOARD_SIZE * BOARD_SIZE> getMask(std::ifstream &file)
    {
        std::bitset<BOARD_SIZE * BOARD_SIZE> mask;
        std::string buffer;

        while (std::getline(file, buffer)) {
            if (buffer == "endmask") {
                break;
            }
            if (buffer.starts_with("#")) {
                continue;
            }
            std::stringstream ss(buffer);
            std::size_t x = 0;
            std::size_t y = 0;
            ss >> x >> y;
            mask.set(GET_INDEX(BOARD_SIZE, x, y));
        }
        return mask;
    }

    static float getScore(std::ifstream &file)
    {
        std::string buffer;
        std::getline(file, buffer);
        std::stringstream ss(buffer);
        float score;
        ss >> score;
        return score;
    }

    std::size_t sizeX;
    std::size_t sizeY;
    std::bitset<BOARD_SIZE * BOARD_SIZE> dataPlayer;
    std::bitset<BOARD_SIZE * BOARD_SIZE> dataOpponent;
    std::bitset<BOARD_SIZE * BOARD_SIZE> mask;
    std::string description;
    float score;
};
