// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.

#include "example_plugin.h"

#include <endstone/color_format.h>
#include <endstone/command/plugin_command.h>
#include <endstone/event/server/server_command_event.h>
#include <endstone/event/server/server_load_event.h>

namespace ExamplePlugin {

Entry* Entry::getInstance() {
    static Entry* instance;
    if (!instance) instance = new Entry();
    return instance;
}

void Entry::onLoad() { getLogger().info("onLoad is called"); }

void Entry::onEnable() {
    getLogger().info("onEnable is called");

    if (auto* command = getCommand("fibonacci")) {
        command->setExecutor(std::make_unique<FibonacciCommandExecutor>());
    }

    // You can register an event handler directly in the Plugin class.
    registerEvent(&Entry::onServerLoad, *this);

    // You can also create a separate class (e.g. ExampleListener) and register the event handler from that class.
    mListener = std::make_unique<ExampleListener>();
    registerEvent(&ExampleListener::onServerLoad, *mListener, endstone::EventPriority::High);
}

void Entry::onDisable() { getLogger().info("onDisable is called"); }

bool Entry::onCommand(
    endstone::CommandSender&        sender,
    const endstone::Command&        command,
    const std::vector<std::string>& args
) {
    // You can also handle commands here instead of setting an executor in onEnable if you prefer
    if (command.getName() == "whoami") {
        if (sender.isOp()) {
            sender.sendMessage(endstone::ColorFormat::DarkGreen + "You are seeing this because you are the OP!");
        } else {
            sender.sendErrorMessage("You should never see this!");
        }
        return true;
    }

    sender.sendErrorMessage("Unknown command: /{}", command.getName());
    return false;
}

void Entry::onServerLoad(endstone::ServerLoadEvent& event) {
    getLogger().info("{} is passed to ExamplePlugin::onServerLoad", event.getEventName());
}

endstone::PluginDescription const& Entry::getDescription() const { return mDescription; }

} // namespace ExamplePlugin

void ExampleListener::onServerLoad(endstone::ServerLoadEvent& event) {
    ExamplePlugin::Entry::getInstance()->getLogger().info("ServerLoadEvent is passed to ExampleListener::onServerLoad");
}

bool FibonacciCommandExecutor::onCommand(
    endstone::CommandSender&        sender,
    endstone::Command const&        command,
    std::vector<std::string> const& args
) {
    if (!command.testPermission(sender)) {
        return true;
    }

    int n = std::stoi(args[0]);

    if (n <= 0) {
        sender.sendErrorMessage("'n' must be an integer greater than 0.");
        return true;
    }

    if (n >= 1000 && !sender.hasPermission("cpp_example.command.fibonacci.large_n")) {
        sender.sendErrorMessage("Only operator can use this command with n >= 1000.");
        return true;
    }

    int         a = 0, b = 1;
    std::string result;
    while (a < n) {
        if (!result.empty()) {
            result += ", ";
        }

        result   += std::to_string(a);
        int temp  = b;
        b         = a + b;
        a         = temp;
    }
    sender.sendMessage("Fibonacci series up to {}: {}", n, result);
    return true;
}

#if defined(WIN32) || defined(_WIN32)
#define EXPORT_ENTRY_POINT __declspec(dllexport)
#else
#define EXPORT_ENTRY_POINT __attribute__((visibility("default")))
#endif
extern "C" [[maybe_unused]] EXPORT_ENTRY_POINT endstone::Plugin* init_endstone_plugin() {
    return ExamplePlugin::Entry::getInstance();
}