#include "Entity.h"

#include <algorithm>
#include <sstream>

Entity::Entity(const std::string& id, float initialX, float initialY, int initialHealth)
    : id(id), x(initialX), y(initialY), health(initialHealth) {
}

const std::string& Entity::getId() const {
    return id;
}

float Entity::getX() const {
    return x;
}

float Entity::getY() const {
    return y;
}

int Entity::getHealth() const {
    return health;
}

void Entity::setX(float x) {
    this->x = x;
}

void Entity::setY(float y) {
    this->y = y;
}

void Entity::setHealth(int health) {
    this->health = health;
}

void Entity::move(float dx, float dy) {
    x += dx;
    y += dy;
}

void Entity::takeDamage(int amount) {
    if (amount > 0) {
        health = amount >= health ? 0 : health - amount;
    }
}

std::string Entity::toString() const {
    std::ostringstream output;
    output << "(" << x << "," << y << ") HP:" << health;
    return output.str();
}
