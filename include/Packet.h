#pragma once

#include <string>

/**
 * Represents one authoritative server message: at this tick, the real command
 * received by the client was the supplied label.
 */
struct Packet {
    int tick;
    std::string label;
};
