#pragma once

#include "GameStateSnapshot.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

/**
 * Maintains the bounded, recent history needed for rollback and visualization.
 *
 * Older confirmed nodes may be evicted, but the most recent confirmed base
 * and all pending predictions remain available for correction. The target
 * window may be exceeded while authoritative confirmation is stalled.
 */
class StateBuffer {
public:
    /**
     * Creates a buffer with a visible history window of the requested size.
     */
    explicit StateBuffer(std::size_t maxWindowSize = 6);

    /**
     * Appends a node and evicts only safe, oldest confirmed history if needed.
     */
    void addNode(std::shared_ptr<GameStateSnapshot> node);

    /**
     * Removes predicted nodes at or after a rollback tick.
     */
    void discardPredictedFrom(int tickNumber);

    /**
     * Finds the newest confirmed node while preserving shared ownership.
     */
    std::shared_ptr<GameStateSnapshot> getLastConfirmedNode() const;

    /**
     * Returns the newest node in the window, regardless of prediction status.
     */
    std::shared_ptr<GameStateSnapshot> getLastNode() const;

    /**
     * Finds a snapshot by tick so authoritative packets can target its node.
     */
    std::shared_ptr<GameStateSnapshot> getNodeAtTick(int tick) const;

    /**
     * Provides read-only access to the ordered window for display purposes.
     */
    const std::vector<std::shared_ptr<GameStateSnapshot>>& getWindow() const;

    /**
     * Confirms every node when the predicted chain has been validated.
     */
    void confirmAllPredicted();

    /**
     * Confirms all snapshots through a tick after authoritative validation.
     */
    void confirmUpTo(int tick);

    /**
     * Renders nodes in order with each incoming command label between them.
     */
    std::string renderChain() const;

private:
    std::vector<std::shared_ptr<GameStateSnapshot>> nodes;
    std::size_t maxWindowSize;
};
