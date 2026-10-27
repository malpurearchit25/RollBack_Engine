#include "AttackCommand.h"

#include <string>

AttackCommand::AttackCommand(int damage)
    : damage(damage) {
}

void AttackCommand::execute(GameStateSnapshot& state) {
    state.getEntities()[0].takeDamage(damage);
}

std::string AttackCommand::getLabel() const {
    return "attack" + std::to_string(damage);
}
