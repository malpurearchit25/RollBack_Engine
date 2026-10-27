#pragma once

#include "Entity.h"

#include <cstddef>
#include <string>
#include <vector>

/**
 * Stores the complete entity state at one simulation tick.
 *
 * Snapshots form the historical nodes needed to compare predictions with
 * confirmed state and replay commands after a rollback.
 */
class GameStateSnapshot {
public:
    /**
     * Creates a snapshot containing the supplied entity state for a tick.
     */
    GameStateSnapshot(int tickNumber, const std::vector<Entity>& entities);

    /**
     * Copies a snapshot and independently copies every entity value.
     *
     * std::vector<Entity> performs value-based copying, so this is a true deep
     * copy for Entity's value members; no shared pointers or references exist.
     * Later edits to this snapshot therefore cannot mutate earlier history.
     */
    GameStateSnapshot(const GameStateSnapshot& other);

    /**
     * Declaring copy assignment alongside the explicit copy constructor follows
     * the Rule of Three and preserves deep-copy snapshot assignment semantics.
     */
    GameStateSnapshot& operator=(const GameStateSnapshot& other) = default;

    /**
     * Returns the tick represented by this snapshot.
     */
    int getTick() const;

    /**
     * Returns mutable access to the entities stored in this snapshot.
     */
    std::vector<Entity>& getEntities();

    /**
     * Returns read-only access to the entities stored in this snapshot.
     */
    const std::vector<Entity>& getEntities() const;

    /**
     * Indicates whether this node is an unconfirmed client prediction.
     */
    bool isPredictedNode() const;

    /**
     * Returns the command label associated with this state transition.
     */
    const std::string& getLastCommandLabel() const;

    /**
     * Marks this node as predicted or confirmed.
     */
    void setPredicted(bool predicted);

    /**
     * Records the short command label that produced this snapshot.
     */
    void setLastCommandLabel(const std::string& commandLabel);

    /**
     * Combines entity values into a lightweight hash for state comparisons.
     */
    std::size_t computeSimpleHash() const;

    /**
     * Produces one compact node string for visualizing a snapshot chain.
     */
    std::string toChainString() const;

private:
    int tickNumber;
    std::vector<Entity> entities;
    bool isPredicted;
    std::string lastCommandLabel;
};
