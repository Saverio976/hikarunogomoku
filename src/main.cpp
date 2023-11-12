#include "Protocol.hpp"
// #include "Pattern20.hpp"
#include <iostream>

int main()
{
    // for (const auto &pattern : Pattern20::getPatterns()) {
    //     std::cout << pattern.getDescription() << std::endl;
    //     std::cout << pattern.getScore() << std::endl;
    //     std::cout << pattern.getSizeX() << " " << pattern.getSizeY() << std::endl;
    //     std::cout << pattern.getDataPlayer().to_string() << std::endl;
    //     std::cout << pattern.getDataOpponent().to_string() << std::endl;
    //     std::cout << pattern.getMask().to_string() << std::endl;
    //     std::cout << std::endl;
    // }
    Protocol::start();
    while (Protocol::getState() != Protocol::State::END) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    Protocol::stop();
    return 0;
}
