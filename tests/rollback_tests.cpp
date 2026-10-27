#include "SimulationEngine.h"
#include <iostream>
#include <stdexcept>
#include <climits>
static void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
int main() {
    try {
        require(!parseCommand("attack-10"), "negative damage accepted");
        require(!parseCommand("attack+10"), "signed damage accepted");
        require(!parseCommand("attack10junk"), "trailing input accepted");
        require(!parseCommand("attack999999999999999"), "overflow accepted");
        require(bool(parseCommand("attack10")), "valid attack rejected");
        Entity entity("test", 0, 0, 100);
        entity.takeDamage(INT_MIN);
        require(entity.getHealth() == 100, "negative damage changed health");
        entity.takeDamage(INT_MAX);
        require(entity.getHealth() == 0, "damage not clamped");
        SimulationEngine engine;
        for (int i = 0; i < 20; ++i) engine.tick();
        require(!engine.receivePacket({2, "d"}), "gap accepted");
        require(engine.receivePacket({1, "w"}), "confirmed base lost during delay");
        require(!engine.receivePacket({1, "a"}), "duplicate accepted");
        require(engine.receivePacket({2, "w"}), "ordered packet rejected");
        require(engine.receivePacket({3, "attack30"}), "attack rejected");
        require(engine.getDisplayChain().find("HP:70") != std::string::npos,
                "one-shot attack replayed repeatedly");
        require(engine.getRollbackCount() == 2, "unexpected rollback count");
        require(engine.getFinalSummary().find("Total ticks simulated: 20") != std::string::npos,
                "incorrect tick count");
        std::cout << "All rollback regressions passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\n";
        return 1;
    }
}
