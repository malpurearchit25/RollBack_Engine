#include "GameStateSnapshot.h"

#include <functional>
#include <sstream>

GameStateSnapshot::GameStateSnapshot(int tickNumber, const std::vector<Entity>& entities)
    : tickNumber(tickNumber), entities(entities), isPredicted(false), lastCommandLabel() {
}

GameStateSnapshot::GameStateSnapshot(const GameStateSnapshot& other)
    : tickNumber(other.tickNumber),
      entities(other.entities),
      isPredicted(other.isPredicted),
      lastCommandLabel(other.lastCommandLabel) {
}

int GameStateSnapshot::getTick() const {
    return tickNumber;
}

std::vector<Entity>& GameStateSnapshot::getEntities() {
    return entities;
}

const std::vector<Entity>& GameStateSnapshot::getEntities() const {
    return entities;
}

bool GameStateSnapshot::isPredictedNode() const {
    return isPredicted;
}

const std::string& GameStateSnapshot::getLastCommandLabel() const {
    return lastCommandLabel;
}

void GameStateSnapshot::setPredicted(bool predicted) {
    isPredicted = predicted;
}

void GameStateSnapshot::setLastCommandLabel(const std::string& commandLabel) {
    lastCommandLabel = commandLabel;
}

std::size_t GameStateSnapshot::computeSimpleHash() const {
    std::size_t hash = 0;

    // This non-cryptographic hash is only for detecting state differences, not security.
    for (const Entity& entity : entities) {
        hash ^= std::hash<float>()(entity.getX()) + 0x9e3779b9
            + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(entity.getY()) + 0x9e3779b9
            + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>()(entity.getHealth()) + 0x9e3779b9
            + (hash << 6) + (hash >> 2);
    }

    return hash;
}

std::string GameStateSnapshot::toChainString() const {
    std::ostringstream output;
    output << "[N" << tickNumber;
    if (isPredicted) {
        output << "(P)";
    }

    output << ": ";
    if (entities.empty()) {
        output << "<empty>";
    } else {
        output << entities[0].toString();
    }
    output << "]";
    return output.str();
}
