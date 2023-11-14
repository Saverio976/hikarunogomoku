#include "Protocol.hpp"
#include "GomukuIA.hpp"
// #include "Pattern20.hpp"

static GomukuBoard board;
static GomukuAI ia(3);

static std::pair<int, int> getMove() {
    auto [x, y] = ia.findBestMove(board);
    board.set(x, y, true);
    return {x, y};
}

static std::pair<int, int> getMove(int x, int y) {
    board.set(x, y, false);
    auto [x2, y2] = ia.findBestMove(board);
    board.set(x2, y2, true);
    return {x2, y2};
}

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
    return 0;
}
