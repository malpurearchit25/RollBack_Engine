#pragma once

#include "ICommand.h"

/**
 * Represents an attack that reduces the first entity's health.
 *
 * Storing the damage as command data makes the action deterministic when it
 * is replayed against a corrected historical snapshot.
 */
class AttackCommand : public ICommand {
public:
    /**
     * Creates an attack command with the specified damage amount.
     */
    explicit AttackCommand(int damage);

    /**
     * Applies this attack's damage to the first entity in the snapshot.
     */
    void execute(GameStateSnapshot& state) override;

    /**
     * Returns a label containing the exact damage amount for chain displays.
     */
    std::string getLabel() const override;

private:
    int damage;
};
