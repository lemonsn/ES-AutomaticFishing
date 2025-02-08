// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.

#pragma once

#include <endstone/command/command.h>
#include <endstone/command/command_executor.h>
#include <string>

class FibonacciCommandExecutor : public endstone::CommandExecutor {
public:
    bool
    onCommand(endstone::CommandSender& sender, endstone::Command const& command, std::vector<std::string> const& args)
        override;
};