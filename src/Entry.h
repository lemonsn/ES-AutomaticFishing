#pragma once
#include <endstone/plugin/plugin.h>

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

    endstone::PluginDescription const& getDescription() const override { return mDescription; }

private:
    PluginDescriptionBuilderImpl mBuilder;
    endstone::PluginDescription  mDescription = mBuilder.build("automatic_fishing", "1.0.5");
};
} // namespace AutomaticFishing