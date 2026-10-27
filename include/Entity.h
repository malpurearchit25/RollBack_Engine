#pragma once

#include <string>

/**
 * Represents one actor whose state can be predicted, corrected, and rewound.
 *
 * Keeping the state private ensures that all changes go through controlled
 * operations, which makes historical snapshots safe to reason about.
 */
class Entity {
public:
    /**
     * Creates an entity with its identity, position, and starting health.
     */
    Entity(const std::string& id, float initialX, float initialY, int initialHealth = 100);

    /**
     * Returns the stable identifier used to distinguish this entity.
     */
    const std::string& getId() const;

    /**
     * Returns the entity's current horizontal position.
     */
    float getX() const;

    /**
     * Returns the entity's current vertical position.
     */
    float getY() const;

    /**
     * Returns the entity's current health value.
     */
    int getHealth() const;

    /**
     * Updates the entity's horizontal position.
     */
    void setX(float x);

    /**
     * Updates the entity's vertical position.
     */
    void setY(float y);

    /**
     * Updates health while leaving validation policy to the caller.
     */
    void setHealth(int health);

    /**
     * Applies a movement delta so prediction commands can update position.
     */
    void move(float dx, float dy);

    /**
     * Applies damage while enforcing the simulation's zero-health floor.
     */
    void takeDamage(int amount);

    /**
     * Produces a compact position and health representation for state displays.
     */
    std::string toString() const;

private:
    std::string id;
    float x;
    float y;
    int health;
};
