#include "SimulationEngine.h"

#include <iostream>
#include <sstream>
#include <vector>

SimulationEngine::SimulationEngine()
    : buffer(),
      rollbackManager(),
      lastKnownCommand(std::make_shared<NoOpCommand>()),
      currentTick(0) {
    std::vector<Entity> entities;
    entities.emplace_back("Player1", 0.0f, 0.0f, 100);
    auto initialState = std::make_shared<GameStateSnapshot>(0, entities);
    initialState->setPredicted(false);
    initialState->setLastCommandLabel("start");
    buffer.addNode(initialState);
}

int SimulationEngine::tick() {
    ++currentTick;
    const std::shared_ptr<ICommand> predictionCommand =
        lastKnownCommand->isRepeatable()
            ? lastKnownCommand
            : std::make_shared<NoOpCommand>();
    const std::shared_ptr<GameStateSnapshot> previous = buffer.getLastNode();
    const GameStateSnapshot copiedState(*previous);
    auto predicted = std::make_shared<GameStateSnapshot>(
        currentTick,
        copiedState.getEntities());
    predictionCommand->execute(*predicted);
    predicted->setPredicted(true);
    predicted->setLastCommandLabel(predictionCommand->getLabel());
    buffer.addNode(predicted);
    return currentTick;
}

bool SimulationEngine::receivePacket(const Packet& packet) {
    // Command-only packets require contiguous authoritative history.
    // Reject duplicates and gaps; callers may retry a gap after its predecessor.
    if (packet.tick != lastConfirmedTick + 1 || packet.tick > currentTick) {
        return false;
    }
    const std::shared_ptr<ICommand> command = parseCommand(packet.label);
    if (command == nullptr) {
        std::cout << "Invalid packet command: " << packet.label << "\n";
        return false;
    }
    if (buffer.getNodeAtTick(packet.tick) == nullptr) {
        std::cout << "Stale packet ignored: tick " << packet.tick << "\n";
        return false;
    }

    lastKnownCommand = command;
    const std::shared_ptr<ICommand> continuation =
        command->isRepeatable()
            ? command
            : std::make_shared<NoOpCommand>();

    try {
        rollbackManager.verify(buffer, packet);
        std::cout << "VERIFIED tick " << packet.tick << "\n";
    } catch (const StateDesyncException& error) {
        std::cout << "MISMATCH: " << error.what() << "\n";
        rollbackManager.performRollback(
            buffer,
            error,
            currentTick,
            continuation);
    }
    lastConfirmedTick = packet.tick;
    return true;
}

std::string SimulationEngine::getDisplayChain() const {
    return buffer.renderChain();
}

int SimulationEngine::getRollbackCount() const {
    return rollbackManager.getRollbackCount();
}

std::string SimulationEngine::getFinalSummary() const {
    std::ostringstream output;
    const std::shared_ptr<GameStateSnapshot> lastNode =
        buffer.getLastConfirmedNode();

    output << "Final state: ";
    if (lastNode == nullptr) {
        output << "<none>";
    } else {
        output << lastNode->toChainString();
    }
    output << "\nTotal ticks simulated: " << currentTick
           << "\nTotal rollbacks triggered: "
           << rollbackManager.getRollbackCount();
    return output.str();
}
