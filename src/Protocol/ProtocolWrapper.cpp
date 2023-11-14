#include "ProtocolWrapper.hpp"
#include "GomukuIA.hpp"
#include "Protocol.hpp"
#include "GomukuBoard.hpp"

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

ProtocolWrapper::ProtocolWrapper()
{
    Protocol::addCommandListener(Protocol::Command::BEGIN, &ProtocolWrapper::sendMove);
    Protocol::addCommandListener(Protocol::Command::TURN, &ProtocolWrapper::sendMove);
    Protocol::addCommandListener(Protocol::Command::BOARD, &ProtocolWrapper::sendMove);
    Protocol::addCommandListener(Protocol::Command::START, &ProtocolWrapper::sendStart);
    Protocol::addCommandListener(Protocol::Command::ABOUT, &ProtocolWrapper::sendAbout);
    Protocol::start();
}

ProtocolWrapper::~ProtocolWrapper()
{
    Protocol::stop();
}

void ProtocolWrapper::sendMove(Protocol::Command command)
{
    std::pair<int, int> posPair;

    if (command == Protocol::Command::TURN) {
        auto pos = Protocol::getTurnArguments();
        posPair = getMove(pos.x, pos.y);
        Protocol::sendTurnResponse(posPair.first, posPair.second);
        return;
    } else if (command == Protocol::Command::BEGIN) {
        posPair = getMove();
        Protocol::sendBeginResponse(posPair.first, posPair.second);
        return;
    } else if (command == Protocol::Command::BOARD) {
        Protocol::sendError("Invalid command");
        // TODO:
    } else {
        Protocol::sendError("Invalid command");
        return;
    }
}

void ProtocolWrapper::sendStart(Protocol::Command command)
{
    Protocol::sendStartResponse(Protocol::Message(Protocol::Status::OK));
    // TODO:
}

void ProtocolWrapper::sendAbout(Protocol::Command command)
{
    Protocol::sendAboutResponse("hikarunogomoku", "1.0.0", "TX", "France");
}
