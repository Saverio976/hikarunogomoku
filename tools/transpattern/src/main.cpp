#include <cstddef>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "fstream"
#include "Pattern.hpp"

constexpr int BOARD_SIZE = 20;

std::vector<std::string> getFilesPatterns(const std::string &inputFolder, const std::string boardSize)
{
    std::vector<std::string> filesPatterns;
    for (const auto &entry : std::filesystem::directory_iterator(inputFolder)) {
        if (entry.is_regular_file() && entry.path().string().ends_with(boardSize + ".txt")) {
            filesPatterns.push_back(entry.path().string());
        }
    }
    return filesPatterns;
}

std::vector<Pattern<BOARD_SIZE>> getPatterns(const std::vector<std::string> &filesPatterns)
{
    std::vector<Pattern<BOARD_SIZE>> patterns;
    for (const auto &filePattern : filesPatterns) {
        patterns.push_back(Pattern<BOARD_SIZE>::fromFile(filePattern));
    }
    return patterns;
}

void writeDefinition(const std::string &pathFolder, const std::string boardSize)
{
    std::string fileName = "Pattern" + boardSize + ".hpp";
    std::ofstream out((std::filesystem::path(pathFolder) / fileName).string());
    out << Pattern<BOARD_SIZE>::definitionToString();
}

void writeImplementation(const std::string &pathFolder, const std::vector<Pattern<BOARD_SIZE>> &patterns, const std::string &boardSize)
{
    std::string fileName = "Pattern" + boardSize + ".cpp";
    std::ofstream out((std::filesystem::path(pathFolder) / fileName).string());
    out << Pattern<BOARD_SIZE>::implementationToString(patterns);
}

int main(int argc, char *argv[])
{
    std::string inputFolder;
    std::string outputFolder;
    std::string boardSizeS;
    std::size_t boardSize;

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <boardSize> <inputFolder> <outputFolder>" << std::endl;
        return 84;
    }
    boardSizeS = argv[1];
    boardSize = std::stoul(boardSizeS);
    if (boardSize != BOARD_SIZE) {
        std::cerr << "Board size must be " << BOARD_SIZE << std::endl;
        return 84;
    }
    inputFolder = argv[2];
    outputFolder = argv[3];
    if (!std::filesystem::exists(inputFolder)) {
        std::cerr << "Input folder does not exist" << std::endl;
        return 84;
    }
    if (!std::filesystem::exists(outputFolder)) {
        std::cerr << "Output folder does not exist" << std::endl;
        return 84;
    }
    auto filesPatterns = getFilesPatterns(inputFolder, boardSizeS);
    auto patterns = getPatterns(filesPatterns);
    writeDefinition(outputFolder, boardSizeS);
    writeImplementation(outputFolder, patterns, boardSizeS);
    return 0;
}
