// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.

#pragma once

#include "example_listener.h"
#include "fibonacci_command.h"
#include <endstone/plugin/plugin.h>

namespace ExamplePlugin {

class PluginDescriptionBuilderImpl : public endstone::detail::PluginDescriptionBuilder {
public:
    PluginDescriptionBuilderImpl() { // defines the metadata for the plugin
        prefix      = "CppExamplePlugin";
        description = "C++ example plugin for Endstone servers";
        website     = "https://github.com/EndstoneMC/cpp-example-plugin";
        authors     = {"Endstone Developers <hello@endstone.dev>"};

        command("whoami")
            .description("Print the information of command executor.")
            .usages("/whoami")
            .permissions("cpp_example.command.whoami");

        command("fibonacci")
            .description("A simple command that writes the Fibonacci series up to n.")
            .usages("/fibonacci <n: int>")
            .aliases("fib")
            .permissions("cpp_example.command.fibonacci", "cpp_example.command.fibonacci.large_n");

        permission("cpp_example.command")
            .description("Allow users to use all commands provided by this example plugin")
            .children("cpp_example.command.whoami", true)
            .children("cpp_example.command.fibonacci", true);

        permission("cpp_example.command.whoami")
            .description("Allow users to use the whoami command")
            .default_(endstone::PermissionDefault::Operator);

        permission("cpp_example.command.fibonacci")
            .description("Allow users to use the fibonacci command")
            .default_(endstone::PermissionDefault::True)
            .children("cpp_example.command.fibonacci.large_n", true);

        permission("cpp_example.command.fibonacci.large_n")
            .description("Allow users to use the fibonacci command with n >= 1000")
            .default_(endstone::PermissionDefault::Operator);
    }
};

class Entry : public endstone::Plugin {
public:
    static Entry* getInstance();

    void onLoad() override;

    void onEnable() override;

    void onDisable() override;

    endstone::PluginDescription const& getDescription() const override;

    bool
    onCommand(endstone::CommandSender& sender, endstone::Command const& command, std::vector<std::string> const& args)
        override;

    void onServerLoad(endstone::ServerLoadEvent& event);

private:
    PluginDescriptionBuilderImpl     mBuilder;
    endstone::PluginDescription      mDescription = mBuilder.build("cpp_example", "0.4.0");
    std::unique_ptr<ExampleListener> mListener;
};
} // namespace ExamplePlugin