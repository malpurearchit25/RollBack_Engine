#pragma once

#include <exception>
#include <string>

/**
 * Reports a predicted-versus-authoritative command mismatch.
 *
 * This custom exception demonstrates the project's Custom Exceptions
 * requirement and carries tick, predicted, and actual context so a catch block
 * can print a useful rollback diagnostic.
 */
class StateDesyncException : public std::exception {
public:
    /**
     * Creates a desync diagnostic and formats its stable explanatory message.
     */
    StateDesyncException(int tick, std::string predicted, std::string actual);

    /**
     * Returns the stored diagnostic text without throwing while handling errors.
     */
    const char* what() const noexcept override;

    /**
     * Returns the tick where the mismatch occurred.
     */
    int getTick() const;

    /**
     * Returns the command label that was predicted.
     */
    const std::string& getPredictedLabel() const;

    /**
     * Returns the authoritative command label that was received.
     */
    const std::string& getActualLabel() const;

private:
    int tickNumber;
    std::string predictedLabel;
    std::string actualLabel;
    std::string message;
};
