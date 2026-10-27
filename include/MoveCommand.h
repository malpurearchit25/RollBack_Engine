#pragma once

#include "ICommand.h"

/**
 * Represents a movement input that changes the first entity's position.
 *
 * Keeping movement as a command allows the same input to be applied during
 * prediction and replay after a rollback.
 */
class MoveCommand : public ICommand {
public:
    /**
     * Creates a movement command with the requested horizontal and vertical deltas.
     */
    MoveCommand(float dx, float dy);

    /**
     * Moves the first entity in the supplied snapshot by this command's deltas.
     */
    void execute(GameStateSnapshot& state) override;

    /**
     * Returns w/a/s/d for cardinal movement or a descriptive delta label.
     */
    std::string getLabel() const override;

    /**
     * Marks movement as repeatable because held directional input can persist
     * across predicted ticks.
     */
    bool isRepeatable() const override;

private:
    float dx;
    float dy;
};
