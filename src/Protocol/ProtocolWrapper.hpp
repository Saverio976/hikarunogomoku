#pragma once

#include "Protocol.hpp"

class ProtocolWrapper {
public:
    ProtocolWrapper();
    ~ProtocolWrapper();

    static void sendMove(Protocol::Command command);
    static void sendStart(Protocol::Command command);
    static void sendAbout(Protocol::Command command);
    static void endCallback(Protocol::Command command);
    static void onInfo(Protocol::Command command);
};
