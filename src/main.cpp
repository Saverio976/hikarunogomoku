#include "Protocol.hpp"
#include "ProtocolWrapper.hpp"

bool isRunning = true;

#ifndef _WIN32
#include <csignal>
static void signalHandler(int signum)
{
    isRunning = false;
    signal(SIGINT, SIG_DFL);
    signal(SIGKILL, SIG_DFL);
}
#endif

int main()
{
    auto proc = ProtocolWrapper();

#ifndef _WIN32
    signal(SIGINT, signalHandler);
    signal(SIGKILL, signalHandler);
#endif
    while (Protocol::getState() != Protocol::State::END && isRunning) {
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (isRunning == false) {
        ProtocolWrapper::endCallback(Protocol::Command::END);
    }
    return 0;
}
