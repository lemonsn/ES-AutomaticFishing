#include "Entry.h"
#include "ll/Memory.h"
#include <exception>
#include <libhat.hpp>

namespace AutomaticFishing {

Entry* Entry::getInstance() {
    static Entry* instance;
    if (!instance) instance = new Entry();
    return instance;
}

void Entry::onEnable() try {
    auto module = hat::process::get_module("bedrock_server.exe");
    if (!module.has_value()) return getLogger().error("Failed to find bedrock_server.exe");
    auto            moduleData = hat::process::get_module_data(module.value());
    static uint64_t trampVar   = 0;
    auto            targetFunc = PLH::findPattern(
        reinterpret_cast<uint64_t>(moduleData.data()),
        moduleData.size_bytes(),
        // FishingHook::_serverHooked
        "40 53 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 ?? 48 8B D9 48 8B 89 ?? ?? ?? ?? 48 8B "
                   "01 48 8B 80"
    );
    if ((void*)targetFunc == nullptr) return getLogger().error("Failed to find FishingHook::_serverHooked function");
    mServerHookedOrigin = reinterpret_cast<decltype(mServerHookedOrigin)>(targetFunc);
    mHook.emplace(targetFunc, reinterpret_cast<uint64_t>(serverHookedDetour), &trampVar);
    if (!mHook->hook()) return getLogger().error("Failed to hook FishingHook::_serverHooked function");
} catch (std::exception const& e) {
    getLogger().error(e.what());
}

void Entry::onDisable() {
    if (mHook.has_value()) mHook->unHook();
}

endstone::PluginDescription const& Entry::getDescription() const { return mDescription; }

bool Entry::serverHookedDetour(void* self) try {
    auto* selfPlugin = getInstance();
    selfPlugin->mHook->unHook();
    auto result = selfPlugin->mServerHookedOrigin(self);
    selfPlugin->mHook->hook();
    if (result && !ll::memory::dAccess<int>(self, 976)) { // FishingHook::mTimeUntilHooked
        static auto moduleData = hat::process::get_module_data(hat::process::get_module("bedrock_server.exe").value());
        static auto getOwnerFunc        = reinterpret_cast<void* (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            // FishingHook::getOwner
            "48 89 5C 24 10 57 48 83 EC 20 48 8B B9 D8 01 00 00"
        ));
        static auto getSelectedItemFunc = reinterpret_cast<void* (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            // Player::getSelectedItem
            "48 8B 91 C8 05 00 00 80 BA B0 00 00 00 00 75 18 48 8B 8A B8 00 00 00 8B 52 10 48 8B 01 48 8B 40 38 48 FF "
            "25 30 37 C6 01"
        ));
        static auto getItemFunc         = reinterpret_cast<void* (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            // ItemStackBase::getItem
            "48 8B 41 08 48 85 C0 74 04 48 8B 00 C3"
        ));
        static auto isNullFunc          = reinterpret_cast<bool (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            // ItemStackBase::isNull
            "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 83 EC ?? 80 79 ?? ?? 48 8B D9"
        ));
        if (auto* player = reinterpret_cast<void*>(getOwnerFunc(self)); player != nullptr) {
            auto* itemStack = getSelectedItemFunc(player);
            auto* item      = getItemFunc(itemStack);
            ll::memory::virtualCall<void*, void*, void*>(item, 76, itemStack, player); // FishingRodItem::use
            if (!isNullFunc(itemStack)) {
                ll::memory::virtualCall<void*, void*, void*>(item, 76, itemStack, player); // FishingRodItem::use
            }
            ll::memory::virtualCall<void, bool>(player, 165, true); // Player::sendInventory
        }
    }
    return result;
} catch (std::exception const& e) {
    Entry::getInstance()->getLogger().error(e.what());
    return false;
}

} // namespace AutomaticFishing

extern "C" [[maybe_unused]] __declspec(dllexport) endstone::Plugin* init_endstone_plugin() {
    return AutomaticFishing::Entry::getInstance();
}