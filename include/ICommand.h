#pragma once

#include "GameStateSnapshot.h"

#include <string>

/**
 * Defines the Command design-pattern interface for simulation actions.
 *
 * The engine can hold commands as std::shared_ptr<ICommand> and invoke
 * execute() without knowing the concrete command type, demonstrating
 * abstraction and polymorphism during prediction and replay.
 */
class ICommand {
public:
    /**
     * Applies this command's effect to the supplied simulation snapshot.
     */
    virtual void execute(GameStateSnapshot& state) = 0;

    /**
     * Returns a compact label suitable for displaying the command in a chain.
     */
    virtual std::string getLabel() const = 0;

    /**
     * Indicates whether prediction may safely assume this command repeats on
     * future ticks; one-shot actions such as attacks return false.
     */
    virtual bool isRepeatable() const {
        return false;
    }

    /**
     * Allows derived commands to be destroyed safely through an interface pointer.
     */
    virtual ~ICommand() = default;
};
