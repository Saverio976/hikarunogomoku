#include "Protocol.hpp"
#include "ProtocolWrapper.hpp"

bool isRunning = true;

int main()
{
    auto proc = ProtocolWrapper();

    while (Protocol::getState() != Protocol::State::END && isRunning) {
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (isRunning == false) {
        ProtocolWrapper::endCallback(Protocol::Command::END);
    }
    return 0;
}
