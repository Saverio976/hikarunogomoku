#include "Protocol.hpp"
#include "GomukuIA.hpp"

int main()
{
    //Protocol::start();
    //while (Protocol::getState() != Protocol::State::END) {
      //  std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}
    //Protocol::stop();

    GomukuBoard board;

    GomukuAI ai(5);
    auto result = ai.findBestMove(board);
    std::cout << result.first << " " << result.second << std::endl;
    return 0;
}
