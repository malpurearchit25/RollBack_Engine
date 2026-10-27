#pragma once

#include "CommandFactory.h"
#include "ICommand.h"
#include "Packet.h"
#include "StateBuffer.h"
#include "StateDesyncException.h"

#include <memory>
#include <string>

/**
 * Detects prediction mismatches and rebuilds the corrected state branch.
 *
 * This class demonstrates custom exceptions as a control-flow mechanism:
 * verify() detects a desync but deliberately does not catch its exception.
 * SimulationEngine can then decide how to handle the problem, separating
 * detection from recovery in a way that is clear to explain in a viva.
 */
class RollbackManager {
public:
    /**
     * Creates a manager with no rollbacks recorded for the current session.
     */
    RollbackManager();

    /**
     * Compares the snapshot at an authoritative tick with its command label.
     *
     * A mismatch throws StateDesyncException for the caller to handle; a match
     * confirms all state through the authoritative tick.
     */
    bool verify(StateBuffer& buffer, const Packet& packet);

    /**
     * Replaces an incorrect branch and re-simulates later ticks to the present.
     */
    void performRollback(
        StateBuffer& buffer,
        const StateDesyncException& error,
        int presentTick,
        std::shared_ptr<ICommand> continuationCommand);

    /**
     * Returns the number of corrections performed in this session.
     */
    int getRollbackCount() const;

private:
    int rollbackCount;
};
