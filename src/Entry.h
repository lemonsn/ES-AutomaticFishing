#pragma once
#include <endstone/plugin/plugin.h>
#include <polyhook2/Detour/x64Detour.hpp>

namespace AutomaticFishing {

class PluginDescriptionBuilderImpl : public endstone::detail::PluginDescriptionBuilder {
public:
    PluginDescriptionBuilderImpl() {
        prefix      = "AutomaticFishing";
        description = "free your hands, fully automatic fishing";
        website     = "https://github.com/ZMBlocks/ES-AutomaticFishing";
        authors     = {"啥也不会的子沐呀 <1756150362@qq.com>"};
    }
};

class Entry : public endstone::Plugin {
public:
    static Entry* getInstance();

    void onEnable() override;

    void onDisable() override;

    endstone::PluginDescription const& getDescription() const override;

    static bool serverHookedDetour(void* self);

private:
    PluginDescriptionBuilderImpl mBuilder;
    endstone::PluginDescription  mDescription = mBuilder.build("automatic_fishing", "1.0.0");
    bool (*mServerHookedOrigin)(void* self);
    std::optional<PLH::x64Detour> mHook;
};
} // namespace AutomaticFishing