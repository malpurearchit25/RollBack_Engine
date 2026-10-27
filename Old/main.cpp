#include "SimulationEngine.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

/*
 * The console loop collects and cleans user input, delegates every action to
 * SimulationEngine, and displays the resulting sliding-window chain. Keeping
 * simulation behavior out of main makes the rollback system reusable and easy
 * to explain independently from its user interface.
 */

static std::string trim(const std::string& input) {
    const auto first = std::find_if(
        input.begin(),
        input.end(),
        [](unsigned char character) {
            return !std::isspace(character);
        });
    const auto last = std::find_if(
        input.rbegin(),
        input.rend(),
        [](unsigned char character) {
            return !std::isspace(character);
        }).base();

    if (first >= last) {
        return {};
    }

    return std::string(first, last);
}

static std::string toLower(std::string input) {
    std::transform(
        input.begin(),
        input.end(),
        input.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });
    return input;
}

static std::string cleanInput(const std::string& input) {
    std::string cleaned = toLower(trim(input));
    if (cleaned == "attack") {
        cleaned = "attack10";
    }
    return cleaned;
}

int main() {
    std::cout << "=== Deterministic Rollback & State Rewind Engine ===\n"
              << "Valid inputs:\n"
              << "  w / a / s / d     -> move up / left / down / right\n"
              << "  attack            -> attack with default damage 10\n"
              << "  attackN           -> attack with damage N (e.g. attack25)\n"
              << "  stop              -> end the simulation\n\n";

    SimulationEngine engine;
    std::string line;

    while (true) {
        std::cout << "Enter first action: ";
        if (!std::getline(std::cin, line)) {
            std::cout << "\n>>> Simulation terminated by user.\n"
                      << engine.getFinalSummary() << "\n";
            return 0;
        }

        const std::string input = cleanInput(line);
        if (input == "stop") {
            std::cout << ">>> Simulation terminated by user.\n"
                      << engine.getFinalSummary() << "\n";
            return 0;
        }

        if (engine.processFirstInput(input)) {
            std::cout << engine.getDisplayChain() << "\n";
            break;
        }

        std::cout << "Invalid action. Try again.\n";
    }

    while (true) {
        std::cout << "\nEnter next action (real input): ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        const std::string input = cleanInput(line);
        if (input == "stop") {
            break;
        }

        if (!engine.processNextInput(input)) {
            std::cout << "Invalid action. Try again.\n";
            continue;
        }

        std::cout << engine.getDisplayChain() << "\n";
    }

    std::cout << ">>> Simulation terminated by user.\n"
              << engine.getFinalSummary() << "\n";
    return 0;
}
