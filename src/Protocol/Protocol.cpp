#include <array>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <string>
#include "Protocol.hpp"

// ---------------------------------------------------------------------------
// Protocol

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

void Protocol::start()
{
}

void Protocol::stop()
{
}

void Protocol::addCommandListener(Command command, std::function<void(Command)> listener)
{
    _commandListeners[static_cast<std::size_t>(command)] = listener;
}

void Protocol::sendStartResponse(const Message &message)
{
    ADD_WRAPPER_TO_BUFFER_SEND(
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
    ADD_WRAPPER_TO_BUFFER_SEND(
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(x).data());
        ADD_TO_CURRENT_BUFFER_SEND(" ");
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(y).data());
    )
}

void Protocol::sendBeginResponse(int x, int y)
{
    ADD_WRAPPER_TO_BUFFER_SEND(
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(x).data());
        ADD_TO_CURRENT_BUFFER_SEND(" ");
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(y).data());
    )
}

void Protocol::sendBoardResponse(int x, int y)
{
    ADD_WRAPPER_TO_BUFFER_SEND(
        ADD_TO_CURRENT_BUFFER_SEND(std::to_string(x).data());
        ADD_TO_CURRENT_BUFFER_SEND(" ");
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
    ADD_WRAPPER_TO_BUFFER_SEND(
        ADD_TO_CURRENT_BUFFER_SEND_3("name=\"", name.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", version=\"", version.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", author=\"", author.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", country=\"", country.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", www=\"", www.data(), "\"");
        ADD_TO_CURRENT_BUFFER_SEND_3(", email=\"", email.data(), "\"");
    )
}

void Protocol::listenAndSendThreaded()
{
    bool isRunning = true;
    static std::string bufferReceive;
    bufferReceive.reserve(ProtocolConfig::MAX_BUFFER_COMMAND_SEND);

    while (isRunning)
    {
        switch (_state) {
            case State::WAITING_BRAIN_RESPONSE:
                std::cin >> bufferReceive;
                if (bufferReceive == "") {
                    continue;
                }
                understandReceiveString(bufferReceive);
                break;
            case State::WAITING_MANAGER_COMMAND:
                sendResponses();
                break;
            default:
                break;
        }
        _inputOutputMutex.lock();
        isRunning = _state == State::END;
        _inputOutputMutex.unlock();
    }
}

void Protocol::understandReceiveString(const std::string &bufferReceive)
{
    bool changeState = true;

    if (_internalState == InternalState::IN_BOARD_COMMAND) {
        changeState = understandInCommandBoard(bufferReceive);
    } else if (bufferReceive.starts_with("START ")) {
        _commandListeners[static_cast<std::size_t>(Command::START)](Command::START);
    } else if (bufferReceive.starts_with("TURN ")) {
        _lastTurnPositions[0] = Protocol::Position(bufferReceive.substr(5));
        if (_lastTurnPositions[0].type == Protocol::Position::Type::NULLPTR) {
            // TODO: Error
            return;
        }
        _commandListeners[static_cast<std::size_t>(Command::TURN)](Command::TURN);
    } else if (bufferReceive.starts_with("BEGIN")) {
        _commandListeners[static_cast<std::size_t>(Command::ABOUT)](Command::BEGIN);
    } else if (bufferReceive.starts_with("BOARD")) {
        for (std::size_t i = 0; i < ProtocolConfig::MAX_NUMBER_TURN; ++i) {
            _lastTurnPositions[i] = Protocol::Position();
        }
        _internalState = InternalState::IN_BOARD_COMMAND;
        changeState = false;
    } else if (bufferReceive.starts_with("INFO ")) {
        _commandListeners[static_cast<std::size_t>(Command::ABOUT)](Command::INFO);
    } else if (bufferReceive.starts_with("ABOUT")) {
        _commandListeners[static_cast<std::size_t>(Command::ABOUT)](Command::ABOUT);
    } else if (bufferReceive.starts_with("END")) {
        changeState = false;
        _inputOutputMutex.lock();
        _state = State::END;
        _inputOutputMutex.unlock();
        _commandListeners[static_cast<std::size_t>(Command::END)](Command::END);
    } else {
    }

    if (changeState) {
        _inputOutputMutex.lock();
        _state = State::WAITING_BRAIN_RESPONSE;
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
        _lastTurnPositions[i] = Protocol::Position(bufferReceive);
        break;
    }
    return false;
}

void Protocol::sendResponses()
{
    MAP_WRAPPER_TO_BUFFER_SEND(
        std::cout << _bufferCommandSend[i].data() << std::endl;
    )
}

void Protocol::defaultListener(Protocol::Command /* unused */)
{
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

Protocol::Position::Position(int x, int y, Type type):
    x(x),
    y(y),
    type(type)
{
}

Protocol::Position::Position(const std::string &position):
    x(0),
    y(0),
    type(Type::NULLPTR)
{
    std::size_t index_first_virgula = position.find(',');
    std::size_t index_second_virgula = position.find(
        ',',
        index_first_virgula + 1);

    if (index_first_virgula == std::string::npos ||
            index_second_virgula == std::string::npos) {
        return;
    }
    const std::string x_str = position.substr(0, index_first_virgula);
    const std::string y_str = position.substr(
        index_first_virgula + 1,
        index_second_virgula - index_first_virgula - 1);

    if (x_str.empty() || y_str.empty() ||
            !std::all_of(x_str.begin(), x_str.end(), ::isdigit) ||
            !std::all_of(y_str.begin(), y_str.end(), ::isdigit)) {
        return;
    }
    x = std::stoi(x_str);
    y = std::stoi(y_str);
    std::size_t type_int = std::stoi(position.substr(index_second_virgula + 1));
    if (type_int == 1) {
        type = Type::ME;
    } else if (type_int == 2) {
        type = Type::OPPONENT;
    }
}

Protocol::Position::Position(const std::string &position, Type type):
    x(0),
    y(0),
    type(type)
{
    std::size_t index_first_virgula = position.find(',');

    if (index_first_virgula == std::string::npos) {
        type = Type::NULLPTR;
        return;
    }
    const std::string x_str = position.substr(0, index_first_virgula);
    const std::string y_str = position.substr(index_first_virgula + 1);

    if (x_str.empty() || y_str.empty() ||
            !std::all_of(x_str.begin(), x_str.end(), ::isdigit) ||
            !std::all_of(y_str.begin(), y_str.end(), ::isdigit)) {
        type = Type::NULLPTR;
        return;
    }
    x = std::stoi(x_str);
    y = std::stoi(y_str);
}

Protocol::Position::Position():
    x(0),
    y(0),
    type(Type::NULLPTR)
{
}
