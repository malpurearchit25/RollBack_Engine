#include "CommandFactory.h"
#include "NoOpCommand.h"

#include <stdexcept>

std::shared_ptr<ICommand> parseCommand(const std::string& input) {
    if (input == "w") {
        return std::make_shared<MoveCommand>(0.0f, 1.0f);
    }
    if (input == "s") {
        return std::make_shared<MoveCommand>(0.0f, -1.0f);
    }
    if (input == "a") {
        return std::make_shared<MoveCommand>(-1.0f, 0.0f);
    }
    if (input == "d") {
        return std::make_shared<MoveCommand>(1.0f, 0.0f);
    }
    if (input == "idle") {
        return std::make_shared<NoOpCommand>();
    }

    constexpr const char* attackPrefix = "attack";
    constexpr std::size_t prefixLength = 6;
    if (input.compare(0, prefixLength, attackPrefix) != 0
        || input.size() == prefixLength) {
        return nullptr;
    }

    const std::string amount = input.substr(prefixLength);
    if (amount.find_first_not_of("0123456789") != std::string::npos) {
        return nullptr;
    }
    try {
        std::size_t parsedCharacters = 0;
        const int damage = std::stoi(
            input.substr(prefixLength),
            &parsedCharacters);

        if (parsedCharacters != input.size() - prefixLength) {
            return nullptr;
        }

        return std::make_shared<AttackCommand>(damage);
    } catch (const std::invalid_argument&) {
        return nullptr;
    } catch (const std::out_of_range&) {
        return nullptr;
    }
}
