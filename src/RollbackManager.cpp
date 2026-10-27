#include "RollbackManager.h"
#include "NoOpCommand.h"

#include <iostream>
#include <memory>
#include <stdexcept>

RollbackManager::RollbackManager()
    : rollbackCount(0) {
}

bool RollbackManager::verify(
    StateBuffer& buffer,
    const Packet& packet) {
    const std::shared_ptr<GameStateSnapshot> node =
        buffer.getNodeAtTick(packet.tick);
    if (node == nullptr) {
        return true;
    }

    if (node->getLastCommandLabel() == packet.label) {
        buffer.confirmUpTo(packet.tick);
        return true;
    }

    throw StateDesyncException(
        packet.tick,
        node->getLastCommandLabel(),
        packet.label);
}

void RollbackManager::performRollback(
    StateBuffer& buffer,
    const StateDesyncException& error,
    int presentTick,
    std::shared_ptr<ICommand> continuationCommand) {
    const int correctionTick = error.getTick();
    buffer.discardPredictedFrom(correctionTick);

    const std::shared_ptr<ICommand> realCommand =
        parseCommand(error.getActualLabel());
    if (realCommand == nullptr) {
        throw std::invalid_argument(
            "Cannot perform rollback: invalid authoritative command label");
    }

    std::shared_ptr<GameStateSnapshot> baseNode =
        buffer.getNodeAtTick(correctionTick - 1);
    if (baseNode == nullptr) {
        baseNode = buffer.getLastConfirmedNode();
    }
    if (baseNode == nullptr) {
        return;
    }

    const GameStateSnapshot baseCopy(*baseNode);
    auto corrected = std::make_shared<GameStateSnapshot>(
        correctionTick,
        baseCopy.getEntities());
    realCommand->execute(*corrected);
    corrected->setPredicted(false);
    corrected->setLastCommandLabel(error.getActualLabel());
    buffer.addNode(corrected);

    int resimulatedTicks = 0;
    std::shared_ptr<GameStateSnapshot> previous = corrected;
    if (continuationCommand == nullptr) {
        continuationCommand = std::make_shared<NoOpCommand>();
    }
    for (int tick = correctionTick + 1; tick <= presentTick; ++tick) {
        const GameStateSnapshot copiedState(*previous);
        auto predicted = std::make_shared<GameStateSnapshot>(
            tick,
            copiedState.getEntities());
        continuationCommand->execute(*predicted);
        predicted->setPredicted(true);
        predicted->setLastCommandLabel(continuationCommand->getLabel());
        buffer.addNode(predicted);
        previous = predicted;
        ++resimulatedTicks;
    }

    ++rollbackCount;
    std::cout << "Rollback at tick " << correctionTick << "\n"
              << "Predicted '" << error.getPredictedLabel()
              << "', actual '" << error.getActualLabel() << "'\n"
              << "Re-simulated " << resimulatedTicks << " tick(s).\n";
}

int RollbackManager::getRollbackCount() const {
    return rollbackCount;
}
