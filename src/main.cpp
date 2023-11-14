#include "Protocol.hpp"
#include "ProtocolWrapper.hpp"

int main()
{
    auto proc = ProtocolWrapper();
    while (Protocol::getState() != Protocol::State::END) {
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
