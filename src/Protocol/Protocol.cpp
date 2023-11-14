#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <algorithm>
#include "Protocol.hpp"

// ---------------------------------------------------------------------------
// Protocol

static const std::string ErrorInvalidPosition = "Invalid Position";
static const std::string ErrorUnknownCommand = "Invalid Unknow Command";

Protocol Protocol::_instance;

std::array<
    std::function<void(Protocol::Command)>,
    static_cast<std::size_t>(Protocol::Command::MAX_MAX_COMMAND_MAX_MAX)
> Protocol::_commandListeners = {
    Protocol::defaultListener,
    Protocol::defaultListener,
    Protocol::defaultListener,
    Protocol::defaultListener,
    Protocol::defaultListener,
    Protocol::defaultListener,
    Protocol::defaultListener,
};

std::mutex Protocol::_inputOutputMutex;

Protocol::State Protocol::_state = Protocol::State::WAITING_MANAGER_COMMAND;

std::array<
    std::array<char, ProtocolConfig::MAX_BUFFER_COMMAND_SEND>,
    ProtocolConfig::MAX_ARRAY_BUFFER_COMMAND_SEND
> Protocol::_bufferCommandSend;

std::array<
    Protocol::Position,
    ProtocolConfig::MAX_NUMBER_TURN
> Protocol::_lastTurnPositions;

Protocol::InternalState Protocol::_internalState =
    Protocol::InternalState::NO_STATE;

std::thread Protocol::_thread;

std::size_t Protocol::_mapStartSize = 0;

void Protocol::start()
{
    std::thread thread(Protocol::listenAndSendThreaded);

    Protocol::_thread.swap(thread);
}

void Protocol::stop()
{
    if (Protocol::getState() != State::END) {
        Protocol::_inputOutputMutex.lock();
        Protocol::_state = Protocol::State::END;
        Protocol::_inputOutputMutex.unlock();
    }
    Protocol::_thread.join();
}

void Protocol::addCommandListener(Command command, std::function<void(Command)> listener)
{
    _commandListeners[static_cast<std::size_t>(command)] = listener;
}

void Protocol::sendStartResponse(const Message &message)
{
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        switch (message.status) {
            case Status::OK:
                ADD_TO_CURRENT_BUFFER_SEND("OK");
                break;
            case Status::ERROR:
                ADD_TO_CURRENT_BUFFER_SEND("ERROR");
                ADD_TO_CURRENT_BUFFER_SEND_2(" ", message.message.data());
                break;
            default:
                break;
        }
    )
}

void Protocol::sendTurnResponse(int x, int y)
{
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(x).data());
        ADD_TO_CURRENT_BUFFER_SEND(",");
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(y).data());
    )
}

void Protocol::sendBeginResponse(int x, int y)
{
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(x).data());
        ADD_TO_CURRENT_BUFFER_SEND(",");
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(y).data());
    )
}

void Protocol::sendBoardResponse(int x, int y)
{
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(x).data());
        ADD_TO_CURRENT_BUFFER_SEND(",");
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(y).data());
    )
}

void Protocol::sendAboutResponse(
    const std::string &name,
    const std::string &version,
    const std::string &author,
    const std::string &country,
    const std::string &www,
    const std::string &email)
{
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        ADD_TO_CURRENT_BUFFER_SEND_3("name=\"", name.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", version=\"", version.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", author=\"", author.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", country=\"", country.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", www=\"", www.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", email=\"", email.data(), "\"");
    )
}

void Protocol::sendUnknown(const std::string &message)
{
    if (Protocol::getState() != State::WAITING_BRAIN_RESPONSE) {
        return;
    }
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        ADD_TO_CURRENT_BUFFER_SEND_2("UNKNOWN ", message.data());
    )
}

void Protocol::sendError(const std::string &message)
{
    if (Protocol::getState() != State::WAITING_BRAIN_RESPONSE) {
        return;
    }
    ADD_WRAPPER_TO_BUFFER_SEND(true,
        ADD_TO_CURRENT_BUFFER_SEND_2("ERROR ", message.data());
    )
}

void Protocol::sendMessage(const std::string &message)
{
    if (Protocol::getState() != State::WAITING_BRAIN_RESPONSE) {
        return;
    }
    ADD_WRAPPER_TO_BUFFER_SEND(false,
        ADD_TO_CURRENT_BUFFER_SEND_2("MESSAGE ", message.data());
    )
}

void Protocol::sendDebug(const std::string &message)
{
    if (Protocol::getState() != State::WAITING_BRAIN_RESPONSE) {
        return;
    }
    ADD_WRAPPER_TO_BUFFER_SEND(false,
        ADD_TO_CURRENT_BUFFER_SEND_2("DEBUG ", message.data());
    )
}

void Protocol::listenAndSendThreaded()
{
    bool isRunning = true;

    while (isRunning)
    {
        std::string bufferReceive;
        switch (_state) {
            case State::WAITING_MANAGER_COMMAND:
                std::getline(std::cin, bufferReceive);
                if (bufferReceive == "") {
                    continue;
                }
                if (bufferReceive.ends_with("\r\n")) {
                    bufferReceive.pop_back();
                    bufferReceive.pop_back();
                } else if (bufferReceive.ends_with("\n")) {
                    bufferReceive.pop_back();
                } else if (bufferReceive.ends_with("\r")) {
                    bufferReceive.pop_back();
                }
                understandReceiveString(bufferReceive);
                bufferReceive.clear();
                break;
            default:
                break;
        }
        sendResponses();
        _inputOutputMutex.lock();
        isRunning = _state != State::END;
        _inputOutputMutex.unlock();
    }
}

std::size_t Protocol::getStartArguments()
{
    return _mapStartSize;
}

void Protocol::understandReceiveString(const std::string &bufferReceive)
{
    bool changeState = true;
    _inputOutputMutex.lock();
    _state = State::WAITING_BRAIN_RESPONSE;
    _inputOutputMutex.unlock();

    if (_internalState == InternalState::IN_BOARD_COMMAND) {
        changeState = understandInCommandBoard(bufferReceive);
        if (changeState) {
            _commandListeners[static_cast<std::size_t>(Command::BOARD)](Command::BOARD);
        }
    } else if (bufferReceive.starts_with("START ")) {
        _mapStartSize = std::stoul(bufferReceive.substr(6));
        _commandListeners[static_cast<std::size_t>(Command::START)](Command::START);
    } else if (bufferReceive.starts_with("TURN ")) {
        _lastTurnPositions[0] = Protocol::Position::fromString(bufferReceive.substr(5), Protocol::Position::Type::OPPONENT);
        if (_lastTurnPositions[0].type == Protocol::Position::Type::NULLPTR) {
            sendError(ErrorInvalidPosition);
            return;
        }
        _commandListeners[static_cast<std::size_t>(Command::TURN)](Command::TURN);
    } else if (bufferReceive.starts_with("BEGIN")) {
        _commandListeners[static_cast<std::size_t>(Command::BEGIN)](Command::BEGIN);
    } else if (bufferReceive.starts_with("BOARD")) {
        for (std::size_t i = 0; i < ProtocolConfig::MAX_NUMBER_TURN; ++i) {
            _lastTurnPositions[i] = Protocol::Position();
        }
        _internalState = InternalState::IN_BOARD_COMMAND;
        changeState = false;
    } else if (bufferReceive.starts_with("INFO ")) {
        ProtocolInfo::setInfo(bufferReceive.substr(5));
        _commandListeners[static_cast<std::size_t>(Command::INFO)](Command::INFO);
    } else if (bufferReceive.starts_with("ABOUT")) {
        _commandListeners[static_cast<std::size_t>(Command::ABOUT)](Command::ABOUT);
    } else if (bufferReceive.starts_with("END")) {
        changeState = false;
        _inputOutputMutex.lock();
        _state = State::END;
        _inputOutputMutex.unlock();
        _commandListeners[static_cast<std::size_t>(Command::END)](Command::END);
    } else {
        sendUnknown(ErrorUnknownCommand + bufferReceive);
        return;
    }

    if (!changeState) {
        _inputOutputMutex.lock();
        _state = State::WAITING_MANAGER_COMMAND;
        _inputOutputMutex.unlock();
    }
}

bool Protocol::understandInCommandBoard(const std::string &bufferReceive)
{
    if (bufferReceive.starts_with("DONE")) {
        _internalState = InternalState::NO_STATE;
        return true;
    }
    for (std::size_t i = 0; i < ProtocolConfig::MAX_NUMBER_TURN; ++i) {
        if (_lastTurnPositions[i].type != Position::Type::NULLPTR) {
            continue;
        }
        _lastTurnPositions[i] = Protocol::Position::fromString(bufferReceive);
        break;
    }
    return false;
}

void Protocol::sendResponses()
{
    MAP_WRAPPER_TO_BUFFER_SEND(
        std::cout << _bufferCommandSend[i].data() << std::endl;
        _bufferCommandSend[i].fill('\0');
    )
}

void Protocol::defaultListener(Protocol::Command command)
{
    sendUnknown(ErrorUnknownCommand + "defaultListener");
}

Protocol::State Protocol::getState()
{
    return _state;
}

const Protocol::Position &Protocol::getTurnArguments()
{
    return _lastTurnPositions[0];
}

const std::array<Protocol::Position, ProtocolConfig::MAX_NUMBER_TURN>
    &Protocol::getBoardArguments()
{
    return _lastTurnPositions;
}

// ---------------------------------------------------------------------------
// Protocol::Message

Protocol::Message::Message(Status status, const std::string &message):
    status(status),
    message(message)
{
}

// ---------------------------------------------------------------------------
// Protocol::Position

Protocol::Position Protocol::Position::fromValues(int x, int y, Type type)
{
    Position pos;

    pos.x = x;
    pos.y = y;
    pos.type = type;
    return pos;
}

Protocol::Position Protocol::Position::fromString(const std::string &position)
{
    Position pos;

    std::size_t index_first_virgula = position.find(',');
    std::size_t index_second_virgula = position.find(
        ',',
        index_first_virgula + 1);

    if (index_first_virgula == std::string::npos ||
            index_second_virgula == std::string::npos) {
        return pos;
    }
    const std::string x_str = position.substr(0, index_first_virgula);
    const std::string y_str = position.substr(
        index_first_virgula + 1,
        index_second_virgula - index_first_virgula - 1);

    if (x_str.empty() || y_str.empty() ||
            !std::all_of(x_str.begin(), x_str.end(), ::isdigit) ||
            !std::all_of(y_str.begin(), y_str.end(), ::isdigit)) {
        return pos;
    }
    pos.x = std::stoi(x_str);
    pos.y = std::stoi(y_str);
    std::size_t type_int = std::stoi(position.substr(index_second_virgula + 1));
    if (type_int == 1) {
        pos.type = Type::ME;
    } else if (type_int == 2) {
        pos.type = Type::OPPONENT;
    } else {
        pos.type = Type::NULLPTR;
    }
    return pos;
}

Protocol::Position Protocol::Position::fromString(const std::string &position, Type type)
{
    Position pos;
    std::size_t index_first_virgula = position.find(',');

    if (index_first_virgula == std::string::npos) {
        return pos;
    }
    const std::string x_str = position.substr(0, index_first_virgula);
    const std::string y_str = position.substr(index_first_virgula + 1);

    if (x_str.empty() || y_str.empty() ||
            !std::all_of(x_str.begin(), x_str.end(), ::isdigit) ||
            !std::all_of(y_str.begin(), y_str.end(), ::isdigit)) {
        return pos;
    }
    pos.x = std::stoi(x_str);
    pos.y = std::stoi(y_str);
    pos.type = type;
    return pos;
}

Protocol::Position::Position():
    x(0),
    y(0),
    type(Type::NULLPTR)
{
}

// ---------------------------------------------------------------------------
// Protocol::Info

static const std::string ErrorGameType = "ErrorGameType";
static const std::string ErrorRule = "ErrorRule";

std::optional<std::size_t> ProtocolInfo::_timeout_turn = std::nullopt;

std::optional<std::size_t> ProtocolInfo::_timeout_match = std::nullopt;

std::optional<std::size_t> ProtocolInfo::_max_memory = std::nullopt;

std::optional<std::size_t> ProtocolInfo::_time_left = std::nullopt;

std::optional<ProtocolInfo::GameType> ProtocolInfo::_game_type = std::nullopt;

std::size_t ProtocolInfo::_rule = 0;

std::optional<std::string> ProtocolInfo::_folder = std::nullopt;

std::optional<std::size_t> ProtocolInfo::getTimeoutTurn()
{
    return ProtocolInfo::_timeout_turn;
}

std::optional<std::size_t> ProtocolInfo::getTimeoutMatch()
{
    return ProtocolInfo::_timeout_match;
}

std::optional<std::size_t> ProtocolInfo::getMaxMemory()
{
    return ProtocolInfo::_max_memory;
}

std::optional<std::size_t> ProtocolInfo::getTimeLeft()
{
    return ProtocolInfo::_time_left;
}

std::optional<ProtocolInfo::GameType> ProtocolInfo::getGameType()
{
    return ProtocolInfo::_game_type;
}

std::optional<bool> ProtocolInfo::isExactFiveInARow()
{
    return static_cast<bool>(
        ProtocolInfo::_rule &
        static_cast<std::size_t>(Rule::EXACT_FIVE_IN_A_ROW));
}

std::optional<bool> ProtocolInfo::isContinuousGame()
{
    return static_cast<bool>(
        ProtocolInfo::_rule &
        static_cast<std::size_t>(Rule::CONTINUOUS_GAME));
}

std::optional<bool> ProtocolInfo::isRenju()
{
    return static_cast<bool>(
        ProtocolInfo::_rule &
        static_cast<std::size_t>(Rule::RENJU));
}

std::optional<bool> ProtocolInfo::isCaro()
{
    return static_cast<bool>(
        ProtocolInfo::_rule &
        static_cast<std::size_t>(Rule::CARO));
}

const std::optional<std::string> &ProtocolInfo::getFolder()
{
    return ProtocolInfo::_folder;
}

void ProtocolInfo::setInfo(const std::string &info)
{
    std::stringstream ss;

    if (info.empty()) {
        return;
    }
    int index_first_space = info.find(' ');
    if (index_first_space == std::string::npos) {
        return;
    }
    std::string value = info.substr(index_first_space + 1);
    ss << value;
    if (info.starts_with("timeout_turn")) {
        std::size_t tmp;
        ss >> tmp;
        ProtocolInfo::_timeout_turn = tmp;
    } else if (info.starts_with("timeout_match")) {
        std::size_t tmp;
        ss >> tmp;
        ProtocolInfo::_timeout_match = tmp;
    } else if (info.starts_with("max_memory")) {
        std::size_t tmp;
        ss >> tmp;
        ProtocolInfo::_max_memory = tmp;
    } else if (info.starts_with("time_left")) {
        std::size_t tmp;
        ss >> tmp;
        ProtocolInfo::_time_left = tmp;
    } else if (info.starts_with("game_type")) {
        if (value == "0") {
            ProtocolInfo::_game_type = GameType::HUMAN_OPPONENT;
        } else if (value == "1") {
            ProtocolInfo::_game_type = GameType::OPPONENT_IS_BRAIN;
        } else if (value == "2") {
            ProtocolInfo::_game_type = GameType::TOURNAMENT;
        } else if (value == "3") {
            ProtocolInfo::_game_type = GameType::NETWORK_TOURNAMENT;
        } else {
            Protocol::sendError(ErrorGameType);
            return;
        }
    } else if (info.starts_with("rule")) {
        if (value != "1" && value != "2" && value != "4" && value != "8") {
            Protocol::sendError(ErrorRule);
            return;
        }
        std::size_t tmp;
        ss >> tmp;
        ProtocolInfo::_rule += tmp;
    } else if (info.starts_with("folder")) {
        ProtocolInfo::_folder = value;
    } else {
        return;
    }
}
