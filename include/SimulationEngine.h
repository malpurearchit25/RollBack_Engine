#pragma once

#include "CommandFactory.h"
#include "Entity.h"
#include "GameStateSnapshot.h"
#include "NoOpCommand.h"
#include "Packet.h"
#include "RollbackManager.h"
#include "StateBuffer.h"
#include "StateDesyncException.h"

#include <memory>
#include <string>

/**
 * Coordinates tick-driven prediction, authoritative packets, and rollback.
 *
 * This class ties the command, snapshot, buffer, factory, and exception
 * abstractions together into the main client-side prediction loop.
 */
class SimulationEngine {
public:
    /**
     * Creates the confirmed starting snapshot and an idle prediction command.
     */
    SimulationEngine();

    /**
     * Advances one tick using the repeatable last known command or idle input.
     */
    int tick();

    /**
     * Validates an authoritative packet and performs correction when needed.
     *
     * This try/catch is where polymorphic ICommand pointers and custom
     * exception handling combine to drive rollback. It is the key method for
     * explaining the project's OOP concepts in a viva.
     */
    bool receivePacket(const Packet& packet);

    /**
     * Returns the current snapshot chain for an external display.
     */
    std::string getDisplayChain() const;

    /**
     * Returns the number of corrections performed so far.
     */
    int getRollbackCount() const;

    /**
     * Builds the final state and activity summary for the simulation session.
     */
    std::string getFinalSummary() const;

private:
    StateBuffer buffer;
    RollbackManager rollbackManager;
    std::shared_ptr<ICommand> lastKnownCommand;
    int currentTick;
    int lastConfirmedTick = 0;
};
