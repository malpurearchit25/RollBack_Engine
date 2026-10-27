#include "CommandFactory.h"
#include "Entity.h"
#include "GameStateSnapshot.h"
#include "NoOpCommand.h"

#include <chrono>
#include <atomic>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "httplib.h"

const int TICK_MS = 500;
const int LATENCY_MS = 1500;
const int PORT = 8080;

struct DelayedPacket {
    int tick;
    std::string label;
    long long deliverAtMs;
};

static std::mutex stateMutex;
static GameStateSnapshot authoritativeState(
    0,
    std::vector<Entity>{Entity("Player1", 0.0f, 0.0f, 100)});
static std::string heldLabel = "idle";
static std::string pendingOneShot;
static int serverTick = 0;
static bool running = false;
static std::atomic<bool> shutdownRequested{false};
static std::deque<DelayedPacket> delayedPackets;
static std::string lastLabel = "idle";

/**
 * Returns monotonic milliseconds for packet delivery scheduling.
 */
static long long nowMilliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

/**
 * Rebuilds the initial authoritative state so a client can start in sync.
 */
static void resetState() {
    authoritativeState = GameStateSnapshot(
        0,
        std::vector<Entity>{Entity("Player1", 0.0f, 0.0f, 100)});
    authoritativeState.setPredicted(false);
    authoritativeState.setLastCommandLabel("start");
    heldLabel = "idle";
    pendingOneShot.clear();
    serverTick = 0;
    delayedPackets.clear();
    lastLabel = "idle";
}

/**
 * Advances the authoritative simulation and queues its delayed packet.
 */
static void runServerTick() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (!running) {
        return;
    }

    const std::string label =
        pendingOneShot.empty() ? heldLabel : pendingOneShot;
    pendingOneShot.clear();

    std::shared_ptr<ICommand> command = parseCommand(label);
    if (command == nullptr) {
        command = std::make_shared<NoOpCommand>();
    }

    const GameStateSnapshot copiedState(authoritativeState);
    GameStateSnapshot nextState(
        serverTick + 1,
        copiedState.getEntities());
    command->execute(nextState);
    nextState.setPredicted(false);
    nextState.setLastCommandLabel(label);
    authoritativeState = nextState;

    // Expire history even when there is no packet consumer.
    while (!delayedPackets.empty()
        && delayedPackets.front().deliverAtMs < nowMilliseconds() - 60000) {
        delayedPackets.pop_front();
    }
    ++serverTick;
    lastLabel = label;
    delayedPackets.push_back(
        {serverTick, label, nowMilliseconds() + LATENCY_MS});

    const Entity& entity = authoritativeState.getEntities()[0];
    std::cout << "[SERVER] tick " << serverTick << ": " << label
              << " -> (" << entity.getX() << "," << entity.getY()
              << ") HP:" << entity.getHealth() << "\n";
}

/**
 * Runs the fixed-rate authoritative clock on its own thread.
 */
static void tickLoop() {
    while (!shutdownRequested.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        runServerTick();
    }
}

/**
 * Adds the required CORS header to every HTTP response.
 */
static httplib::Server::HandlerResponse addCorsHeader(
    const httplib::Request&,
    httplib::Response& response) {
    response.set_header("Access-Control-Allow-Origin", "*");
    response.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response.set_header("Access-Control-Allow-Headers", "Content-Type");
    return httplib::Server::HandlerResponse::Unhandled;
}

/**
 * Escapes a label for the small hand-built JSON response.
 */
static std::string jsonEscape(const std::string& value) {
    std::string escaped;
    for (const char character : value) {
        if (character == '\\' || character == '"') {
            escaped += '\\';
        }
        escaped += character;
    }
    return escaped;
}

/**
 * Starts the authoritative HTTP server and its delayed tick clock.
 */
int main() {
    httplib::Server server;

    server.set_pre_routing_handler(addCorsHeader);
    if (!server.set_mount_point("/", "web")) {
        std::cerr << "Missing web directory. Run the server from the project or build directory.\n";
        return 1;
    }

    server.Post("/start", [](const httplib::Request&, httplib::Response& response) {
        std::lock_guard<std::mutex> lock(stateMutex);
        resetState();
        running = true;
        std::cout << "[SERVER] started\n";
        response.set_content("ok", "text/plain");
    });

    server.Post("/input", [](const httplib::Request& request, httplib::Response& response) {
        const std::string label = request.body;
        if (parseCommand(label) == nullptr) {
            response.status = 400;
            response.set_content("invalid", "text/plain");
            return;
        }

        std::lock_guard<std::mutex> lock(stateMutex);
        if (label.rfind("attack", 0) == 0) {
            pendingOneShot = label;
        } else {
            heldLabel = label;
        }
        std::cout << "[SERVER] input " << label << "\n";
        response.set_content("ok", "text/plain");
    });

    server.Get("/state", [](const httplib::Request&, httplib::Response& response) {
        std::lock_guard<std::mutex> lock(stateMutex);
        const Entity& entity = authoritativeState.getEntities()[0];
        std::ostringstream body;
        body << "{\"tick\":" << serverTick
             << ",\"x\":" << entity.getX()
             << ",\"y\":" << entity.getY()
             << ",\"hp\":" << entity.getHealth()
             << ",\"label\":\"" << jsonEscape(lastLabel)
             << "\",\"running\":" << (running ? "true" : "false") << "}";
        response.set_content(body.str(), "application/json");
    });

    server.Get("/packets", [](const httplib::Request& request, httplib::Response& response) {
        int afterTick = 0;
        if (request.has_param("after")) {
            try {
                afterTick = std::stoi(request.get_param_value("after"));
            } catch (const std::exception&) {
                response.status = 400;
                response.set_content("invalid after", "text/plain");
                return;
            }
        }

        const long long currentMs = nowMilliseconds();
        std::lock_guard<std::mutex> lock(stateMutex);
        while (!delayedPackets.empty()
            && delayedPackets.front().deliverAtMs < currentMs - 60000) {
            delayedPackets.pop_front();
        }

        std::ostringstream body;
        for (const DelayedPacket& packet : delayedPackets) {
            if (packet.tick > afterTick && packet.deliverAtMs <= currentMs) {
                body << packet.tick << " " << packet.label << "\n";
            }
        }
        response.set_content(body.str(), "text/plain");
    });

    std::cout << "[SERVER] listening at http://localhost:" << PORT << "\n";
    std::thread clockThread(tickLoop);
    const bool listened = server.listen("127.0.0.1", PORT);
    shutdownRequested.store(true);
    clockThread.join();
    if (!listened) {
        std::cerr << "Unable to listen on port " << PORT << "\n";
        return 1;
    }
    return 0;
}
