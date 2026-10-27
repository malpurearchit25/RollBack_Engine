#pragma once

#include "ICommand.h"

/**
 * Represents a tick where no input is held.
 *
 * It performs no state change while still participating in the ICommand
 * hierarchy, providing another concrete example of polymorphism.
 */
class NoOpCommand : public ICommand {
public:
    /**
     * Leaves the supplied snapshot unchanged because no input was held.
     */
    void execute(GameStateSnapshot& state) override;

    /**
     * Returns the stable label used to display an idle tick.
     */
    std::string getLabel() const override;

    /**
     * Prevents idle input from being treated as a repeating action.
     */
    bool isRepeatable() const override;
};
