#include "StateBuffer.h"

#include <algorithm>
#include <sstream>
#include <utility>

StateBuffer::StateBuffer(std::size_t maxWindowSize)
    : nodes(), maxWindowSize(maxWindowSize) {
}

void StateBuffer::addNode(std::shared_ptr<GameStateSnapshot> node) {
    nodes.push_back(std::move(node));

    // Keep the last confirmed base until its next transition is confirmed.
    while (nodes.size() > maxWindowSize
        && nodes.size() > 1
        && !nodes[1]->isPredictedNode()) {
        nodes.erase(nodes.begin());
    }
}

void StateBuffer::discardPredictedFrom(int tickNumber) {
    nodes.erase(
        std::remove_if(
            nodes.begin(),
            nodes.end(),
            [tickNumber](const std::shared_ptr<GameStateSnapshot>& node) {
                return node->getTick() >= tickNumber && node->isPredictedNode();
            }),
        nodes.end());
}

std::shared_ptr<GameStateSnapshot> StateBuffer::getLastConfirmedNode() const {
    for (auto iterator = nodes.rbegin(); iterator != nodes.rend(); ++iterator) {
        if (!(*iterator)->isPredictedNode()) {
            return *iterator;
        }
    }

    return nullptr;
}

std::shared_ptr<GameStateSnapshot> StateBuffer::getLastNode() const {
    if (nodes.empty()) {
        return nullptr;
    }

    return nodes.back();
}

std::shared_ptr<GameStateSnapshot> StateBuffer::getNodeAtTick(int tick) const {
    for (const std::shared_ptr<GameStateSnapshot>& node : nodes) {
        if (node->getTick() == tick) {
            return node;
        }
    }

    return nullptr;
}

const std::vector<std::shared_ptr<GameStateSnapshot>>& StateBuffer::getWindow() const {
    return nodes;
}

void StateBuffer::confirmAllPredicted() {
    for (const std::shared_ptr<GameStateSnapshot>& node : nodes) {
        node->setPredicted(false);
    }
}

void StateBuffer::confirmUpTo(int tick) {
    for (const std::shared_ptr<GameStateSnapshot>& node : nodes) {
        if (node->getTick() <= tick) {
            node->setPredicted(false);
        }
    }
}

std::string StateBuffer::renderChain() const {
    std::ostringstream output;

    for (std::size_t index = 0; index < nodes.size(); ++index) {
        if (index > 0) {
            output << " --" << nodes[index]->getLastCommandLabel() << "--> ";
        }
        output << nodes[index]->toChainString();
    }

    return output.str();
}
