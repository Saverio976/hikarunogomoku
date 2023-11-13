#include "Protocol.hpp"
#include "GomukuIA.hpp"
// #include "Pattern20.hpp"

int main()
{
    //Protocol::start();
    //while (Protocol::getState() != Protocol::State::END) {
      //  std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}
    //Protocol::stop();
  
    // for (const auto &pattern : Pattern20::getPatterns()) {
    //     std::cout << pattern.getDescription() << std::endl;
    //     std::cout << pattern.getScore() << std::endl;
    //     std::cout << pattern.getSizeX() << " " << pattern.getSizeY() << std::endl;
    //     std::cout << pattern.getDataPlayer().to_string() << std::endl;
    //     std::cout << pattern.getDataOpponent().to_string() << std::endl;
    //     std::cout << pattern.getMask().to_string() << std::endl;
    //     std::cout << std::endl;
    // }

    GomukuBoard board;

    GomukuAI ai(5);
    auto result = ai.findBestMove(board);
    std::cout << result.first << " " << result.second << std::endl;

    return 0;
}
