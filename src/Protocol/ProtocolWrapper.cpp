#include "ProtocolWrapper.hpp"
#include "GomukuIA.hpp"
#include "Protocol.hpp"
#include "GomukuBoard.hpp"
#include "Perfcounter.hpp"

static GomukuBoard board;
static GomukuAI ia(5);

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

static std::pair<int, int> getMove(const std::array<Protocol::Position, ProtocolConfig::MAX_NUMBER_TURN> &poss) {
    for (auto &pos : poss) {
        if (pos.type == Protocol::Position::Type::NULLPTR) {
            break;
        }
        board.set(pos.x, pos.y, false);
    }
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
    Protocol::addCommandListener(Protocol::Command::END, &ProtocolWrapper::endCallback);
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
        {
            Perfcounter::Counter counter(Perfcounter::PerfType::TIME_ALGO_FULL);
            posPair = getMove(pos.x, pos.y);
        }
        Protocol::sendTurnResponse(posPair.first, posPair.second);
        return;
    } else if (command == Protocol::Command::BEGIN) {
        {
            Perfcounter::Counter counter(Perfcounter::PerfType::TIME_ALGO_FULL);
            posPair = getMove();
        }
        Protocol::sendBeginResponse(posPair.first, posPair.second);
        return;
    } else if (command == Protocol::Command::BOARD) {
        auto poss = Protocol::getBoardArguments();
        {
            Perfcounter::Counter counter(Perfcounter::PerfType::TIME_ALGO_FULL);
            posPair = getMove(poss);
        }
        Protocol::sendBeginResponse(posPair.first, posPair.second);
        return;
    } else {
        Protocol::sendError("Invalid command");
        return;
    }
}

void ProtocolWrapper::sendStart(Protocol::Command command)
{
    if (Protocol::getStartArguments() != 20) {
        Protocol::sendStartResponse(Protocol::Message(Protocol::Status::ERROR, "Only 20x20 board"));
    } else {
        Protocol::sendStartResponse(Protocol::Message(Protocol::Status::OK));
    }
}

void ProtocolWrapper::sendAbout(Protocol::Command command)
{
    Protocol::sendAboutResponse("hikarunogomoku", "1.0.0", "TX", "France");
}

void ProtocolWrapper::endCallback(Protocol::Command command)
{
    Perfcounter::writeStats("stats.txt");
}
