#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <mutex>
#include <string>
#include <cstring>

class ProtocolConfig {
public:
    // Size of the buffer to send.
    static constexpr std::size_t MAX_BUFFER_COMMAND_SEND = 1024;
    // Number of buffer of size `MAX_BUFFER_COMMAND_SEND` that can be sent.
    static constexpr std::size_t MAX_ARRAY_BUFFER_COMMAND_SEND = 3;
    // Number of turn max that can be stored. // TODO: use the same value as ../Board
    static constexpr std::size_t MAX_NUMBER_TURN = 20 * 20;
};

// Map on all buffer to send to the Gomoku Manager.
#define MAP_WRAPPER_TO_BUFFER_SEND(code_block)                                          \
    _inputOutputMutex.lock();                                                           \
    for (std::size_t i = 0; i < ProtocolConfig::MAX_ARRAY_BUFFER_COMMAND_SEND; i++) {   \
        if (_bufferCommandSend[i][0] == '\0') {                                         \
            break;                                                                      \
        }                                                                               \
        code_block                                                                      \
    }                                                                                   \
    _inputOutputMutex.unlock();

// Map on all buffer to send to the Gomoku Manager and stop to the first
// available buffer.
#define ADD_WRAPPER_TO_BUFFER_SEND(code_block)                                          \
    _inputOutputMutex.lock();                                                           \
    for (std::size_t i = 0; i < ProtocolConfig::MAX_ARRAY_BUFFER_COMMAND_SEND; i++) {   \
        if (_bufferCommandSend[i][0] != '\0') {                                         \
            continue;                                                                   \
        }                                                                               \
        code_block                                                                      \
        break;                                                                          \
    }                                                                                   \
    _inputOutputMutex.unlock();

// Add to the current buffer selected by the index `i` the `const char *`
// `data_string` if the buffer can accept the size of `data_string`.
// Better use it with the macro ADD_WRAPPER_TO_BUFFER_SEND.
#define ADD_TO_CURRENT_BUFFER_SEND(data_string)                                         \
    if (std::strlen(data_string) + std::strlen(_bufferCommandSend[i].data()) <          \
            ProtocolConfig::MAX_BUFFER_COMMAND_SEND) {                                  \
        std::strcat(_bufferCommandSend[i].data(), data_string);                         \
    }

// Add to the current buffer selected by the index `i` the `const char *`
// `data_string` if the buffer can accept the size of `data_string` + size of
// `data_string_2`.
// Better use it with the macro ADD_WRAPPER_TO_BUFFER_SEND.
#define ADD_TO_CURRENT_BUFFER_SEND_2(data_string, data_string_2)                        \
    if (std::strlen(data_string) + std::strlen(_bufferCommandSend[i].data()) +          \
            std::strlen(data_string_2) < ProtocolConfig::MAX_BUFFER_COMMAND_SEND) {     \
        std::strcat(_bufferCommandSend[i].data(), data_string);                         \
        std::strcat(_bufferCommandSend[i].data(), data_string_2);                       \
    }

// Add to the current buffer selected by the index `i` the `const char *`
// `data_string` if the buffer can accept the size of `data_string` + size of
// `data_string_2` + size of `data_string_3`.
// Better use it with the macro ADD_WRAPPER_TO_BUFFER_SEND.
#define ADD_TO_CURRENT_BUFFER_SEND_3(data_string, data_string_2, data_string_3)         \
    if (std::strlen(data_string) + std::strlen(_bufferCommandSend[i].data()) +          \
            std::strlen(data_string_2) + std::strlen(data_string_3) <                   \
            ProtocolConfig::MAX_BUFFER_COMMAND_SEND) {                                                  \
        std::strcat(_bufferCommandSend[i].data(), data_string);                         \
        std::strcat(_bufferCommandSend[i].data(), data_string_2);                       \
        std::strcat(_bufferCommandSend[i].data(), data_string_3);                       \
    }

// Protocol Helper to interact with the Gomoku Manager.
class Protocol {
public:
    // The different command that can be received.
    // (The Gomoku Manager can send all this Command).
    enum class Command : std::size_t {
        START = 0,
        TURN,
        BEGIN,
        BOARD,
        INFO,
        END,
        ABOUT,
        MAX_MAX_COMMAND_MAX_MAX,
    };

    // The current state of the Protocol.
    enum class State {
        WAITING_BRAIN_RESPONSE,
        WAITING_MANAGER_COMMAND,
        END,
    };

    // Status of the class Message
    enum class Status {
        OK,
        ERROR,
    };

    // Wrapper for a Status and a string message
    struct Message {
        // If the status is ERROR, the message will be added to the command
        // that will be send to the Gomoku Manager for the Response of START
        // Command.
        Message(Status status, const std::string &message = "");

        Status status;
        std::string message;
    };

    // Wrapper for a X and Y position + the type
    struct Position {
        // The type of the position stone
        enum class Type : std::size_t {
            ME = 1,
            OPPONENT = 2,
            NULLPTR,
        };

        // Create a position with X and Y and Type
        Position(int x, int y, Type type);
        // Create a position in form of "X,Y,Type
        Position(const std::string &position);
        // Create a position in form of "X,Y"
        Position(const std::string &position, Type type);
        Position();

        int x;
        int y;
        Type type;
    };

    Protocol(Protocol const &) = delete;
    void operator=(Protocol const &) = delete;
    Protocol(Protocol &&) = delete;
    void operator=(Protocol &&) = delete;
    ~Protocol() = default;

    // Start the protocol (it will launch a thread that will listen for Command
    // from the Gomoku Manager and that can send Response back).
    static void start();
    // Stop the internal thread launched.
    static void stop();

    // Register the callback that will be called when the Command specified
    // will be received. The function will be called inside the thread created
    // by the `start` static method.
    // Make sure the function is thread-safe.
    static void addCommandListener(
        Command command,
        std::function<void(Command)> listener);

    // Get thet arguments passed to the `TURN` Command.
    // It's the position of the new ennemy stone.
    static const Position &getTurnArguments();
    // Get the arguments passed to the `BOARD` Command.
    // They are all the stone on the board.
    // If the `Position.type` is `NULLPTR`, there is no stone on the position
    // specified.
    // If a `Position.type` is `NULLPTR` and you iterate from index 0 to the
    // end. You can break the loop because there will be no other stone after.
    static const std::array<Position, ProtocolConfig::MAX_NUMBER_TURN>
        &getBoardArguments();

    // Response for the `START` Command.
    static void sendStartResponse(const Message &message);
    // Response for the `TURN` Command.
    static void sendTurnResponse(int x, int y);
    // Response for the `BEGIN` Command.
    static void sendBeginResponse(int x, int y);
    // Response for the `BOARD` Command.
    static void sendBoardResponse(int x, int y);
    // Response for the `ABOUT` Command.
    static void sendAboutResponse(
        const std::string &name,
        const std::string &version,
        const std::string &author,
        const std::string &country = "france",
        const std::string &www = "www.example.com",
        const std::string &email = "example@ex.com");

private:
    // METHODS ---------------------------------------------------------------

    Protocol() = default;

    static void listenAndSendThreaded();

    static void understandReceiveString(const std::string &bufferReceive);
    static bool understandInCommandBoard(const std::string &bufferReceive);

    static void sendResponses();

    static void defaultListener(Command command);

    // ATTRIBUTES ------------------------------------------------------------

    enum class InternalState {
        IN_BOARD_COMMAND,
        NO_STATE,
    };

    static Protocol _instance;

    static std::array<
        std::function<void(Command)>,
        static_cast<std::size_t>(Command::MAX_MAX_COMMAND_MAX_MAX)
    > _commandListeners;

    static std::mutex _inputOutputMutex;

    static State _state;

    static std::array<
        std::array<char, ProtocolConfig::MAX_BUFFER_COMMAND_SEND>,
        ProtocolConfig::MAX_ARRAY_BUFFER_COMMAND_SEND
    > _bufferCommandSend;

    static std::array<
        Position,
        ProtocolConfig::MAX_NUMBER_TURN
    > _lastTurnPositions;

    static InternalState _internalState;
};
