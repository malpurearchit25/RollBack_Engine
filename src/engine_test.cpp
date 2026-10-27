#include "Packet.h"
#include "SimulationEngine.h"

#include <deque>
#include <iostream>
#include <string>

/*
 * This test simulates a delayed authoritative server without networking.
 * The client predicts each tick, queues server packets with fixed latency,
 * and lets SimulationEngine verify or roll back its predicted state.
 */

const int LATENCY_TICKS = 3;
const int TOTAL_TICKS = 14;

static std::string authoritativeCommandFor(int tick) {
    if (tick <= 4) {
        return "w";
    }
    if (tick <= 6) {
        return "d";
    }
    if (tick == 7) {
        return "attack30";
    }
    return "idle";
}

struct QueuedPacket {
    int deliverAtTick;
    Packet packet;
};

int main() {
    SimulationEngine engine;
    std::deque<QueuedPacket> pendingPackets;

    for (int tick = 1; tick <= TOTAL_TICKS; ++tick) {
        engine.tick();

        pendingPackets.push_back(
            {tick + LATENCY_TICKS, Packet{tick, authoritativeCommandFor(tick)}});

        while (!pendingPackets.empty()
            && pendingPackets.front().deliverAtTick <= tick) {
            engine.receivePacket(pendingPackets.front().packet);
            pendingPackets.pop_front();
        }

        std::cout << "Tick " << tick << ":\n"
                  << engine.getDisplayChain() << "\n\n";
    }

    std::cout << engine.getFinalSummary() << "\n";
    return 0;
}
