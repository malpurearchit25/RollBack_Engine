#pragma once

#include "AttackCommand.h"
#include "ICommand.h"
#include "MoveCommand.h"

#include <memory>
#include <string>

/**
 * Converts a textual command label into a concrete command object.
 *
 * This Factory-style helper keeps string parsing separate from the ICommand
 * hierarchy, so MoveCommand and AttackCommand only need to represent and
 * execute their own behavior.
 */
std::shared_ptr<ICommand> parseCommand(const std::string& input);
