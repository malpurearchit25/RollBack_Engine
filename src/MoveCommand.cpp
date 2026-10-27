#include "MoveCommand.h"

#include <sstream>

MoveCommand::MoveCommand(float dx, float dy)
    : dx(dx), dy(dy) {
}

void MoveCommand::execute(GameStateSnapshot& state) {
    state.getEntities()[0].move(dx, dy);
}

std::string MoveCommand::getLabel() const {
    if (dx == 0.0f && dy > 0.0f) {
        return "w";
    }
    if (dx == 0.0f && dy < 0.0f) {
        return "s";
    }
    if (dx < 0.0f && dy == 0.0f) {
        return "a";
    }
    if (dx > 0.0f && dy == 0.0f) {
        return "d";
    }

    std::ostringstream output;
    output << "move(" << dx << "," << dy << ")";
    return output.str();
}

bool MoveCommand::isRepeatable() const {
    return true;
}
