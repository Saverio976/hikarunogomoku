#include "Protocol.hpp"
int main()
{
    Protocol::start();
    while (Protocol::getState() != Protocol::State::END) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    Protocol::stop();
    return 0;
}
