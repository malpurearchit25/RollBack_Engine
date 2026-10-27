#include "StateDesyncException.h"

#include <utility>

StateDesyncException::StateDesyncException(
    int tick,
    std::string predicted,
    std::string actual)
    : tickNumber(tick),
      predictedLabel(std::move(predicted)),
      actualLabel(std::move(actual)),
      message(
          "Desync at tick " + std::to_string(tickNumber)
          + ": predicted '" + predictedLabel
          + "' but actual was '" + actualLabel + "'") {
}

const char* StateDesyncException::what() const noexcept {
    return message.c_str();
}

int StateDesyncException::getTick() const {
    return tickNumber;
}

const std::string& StateDesyncException::getPredictedLabel() const {
    return predictedLabel;
}

const std::string& StateDesyncException::getActualLabel() const {
    return actualLabel;
}
