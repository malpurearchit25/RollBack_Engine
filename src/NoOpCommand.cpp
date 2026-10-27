#include "NoOpCommand.h"

void NoOpCommand::execute(GameStateSnapshot& state) {
    (void)state;
}

std::string NoOpCommand::getLabel() const {
    return "idle";
}

bool NoOpCommand::isRepeatable() const {
    return false;
}
