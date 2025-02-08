#include "Entry.h"
#include "ll/Memory.h"
#include <libhat.hpp>

namespace AutomaticFishing {

Entry* Entry::getInstance() {
    static Entry* instance;
    if (!instance) instance = new Entry();
    return instance;
}

void Entry::onEnable() {
    auto module = hat::process::get_module("bedrock_server.exe");
    if (!module.has_value()) return getLogger().error("Failed to find bedrock_server.exe");
    auto            moduleData = hat::process::get_module_data(module.value());
    static uint64_t trampVar   = 0;
    auto            targetFunc = PLH::findPattern(
        reinterpret_cast<uint64_t>(moduleData.data()),
        moduleData.size_bytes(),
        "40 57 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 ?? 48 8B F9 E8"
    );
    mServerHookedOrigin = reinterpret_cast<decltype(mServerHookedOrigin)>(targetFunc);
    mHook.emplace(targetFunc, reinterpret_cast<uint64_t>(serverHookedDetour), &trampVar);
    if (!mHook->hook()) return getLogger().error("Failed to hook FishingHook::_serverHooked function");
}

void Entry::onDisable() {
    if (mHook.has_value()) mHook->unHook();
}

endstone::PluginDescription const& Entry::getDescription() const { return mDescription; }

bool Entry::serverHookedDetour(void* self) {
    auto* selfPlugin = getInstance();
    selfPlugin->mHook->unHook();
    auto result = selfPlugin->mServerHookedOrigin(self);
    selfPlugin->mHook->hook();
    if (result && !ll::memory::dAccess<int>(self, 976)) {
        static auto moduleData = hat::process::get_module_data(hat::process::get_module("bedrock_server.exe").value());
        static auto getOwnerFunc        = reinterpret_cast<void* (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            "48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 48 8B D9 E8 ?? ?? ?? ?? 48 8B F0"
        ));
        static auto getSelectedItemFunc = reinterpret_cast<void* (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            "80 B9 ?? ?? ?? ?? ?? 48 8B D1 75"
        ));
        static auto getItemFunc         = reinterpret_cast<void* (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            "48 8B 41 08 48 85 C0 74 04 48 8B 00 C3"
        ));
        static auto isNullFunc          = reinterpret_cast<bool (*)(void* self)>(PLH::findPattern(
            reinterpret_cast<uint64_t>(moduleData.data()),
            moduleData.size_bytes(),
            "40 53 48 83 EC ?? 80 79 ?? ?? 48 8B D9 74 ?? 48 8B 41 ?? 48 85 C0"
        ));
        if (auto* player = reinterpret_cast<void*>(getOwnerFunc(self)); player != nullptr) {
            auto* itemStack = getSelectedItemFunc(ll::memory::dAccess<void*>(player, 1480));
            auto* item      = getItemFunc(itemStack);
            ll::memory::virtualCall<void*, void*, void*>(item, 76, itemStack, player);
            if (!isNullFunc(itemStack)) {
                ll::memory::virtualCall<void*, void*, void*>(item, 76, itemStack, player);
            }
            ll::memory::virtualCall<void, bool>(player, 170, true);
        }
    }
    return result;
}

} // namespace AutomaticFishing

#if defined(WIN32) || defined(_WIN32)
#define EXPORT_ENTRY_POINT __declspec(dllexport)
#else
#define EXPORT_ENTRY_POINT __attribute__((visibility("default")))
#endif
extern "C" [[maybe_unused]] EXPORT_ENTRY_POINT endstone::Plugin* init_endstone_plugin() {
    return AutomaticFishing::Entry::getInstance();
}