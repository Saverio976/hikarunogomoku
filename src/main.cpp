#include "Protocol.hpp"
#include <csignal>
#include "ProtocolWrapper.hpp"

bool isRunning = true;

static void signalHandler(int signum)
{
    isRunning = false;
    signal(SIGINT, SIG_DFL);
}

int main()
{
    auto proc = ProtocolWrapper();

    signal(SIGINT, signalHandler);
    while (Protocol::getState() != Protocol::State::END && isRunning) {
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (isRunning == false) {
        ProtocolWrapper::endCallback(Protocol::Command::END);
    }
    return 0;
}
