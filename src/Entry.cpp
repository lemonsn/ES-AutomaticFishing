#include "Entry.h"
#include <glacie/memory/Hook.h>

GLACIE_INSTANCE_HOOK(
    ServerHookedHook,
    "40 53 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 ?? 48 8B D9 48 8B 89 ?? ?? ?? ?? 48 8B 01 48 "
    "8B 80",
    bool
) {
    auto result = origin();
    if (result && !glacie::memory::dAccess<int>(this, 976)) { // FishingHook::mTimeUntilHooked
        // FishingHook::getOwner
        static auto getOwnerFunc = reinterpret_cast<void* (*)(void* self)>(
            glacie::memory::resolveIdentifier("48 89 5C 24 10 57 48 83 EC 20 48 8B B9 D8 01 00 00")
        );

        // Player::getSelectedItem
        static auto getSelectedItemFunc = reinterpret_cast<void* (*)(void* self)>(glacie::memory::resolveIdentifier(
            "48 8B 91 C8 05 00 00 80 BA B0 00 00 00 00 75 18 48 8B 8A B8 00 00 00 8B 52 10 48 8B 01 48 8B 40 38 48 FF "
            "25 30 37 C6 01"
        ));

        // ItemStackBase::getItem
        static auto getItemFunc = reinterpret_cast<void* (*)(void* self)>(
            glacie::memory::resolveIdentifier("48 8B 41 08 48 85 C0 74 04 48 8B 00 C3")
        );

        // ItemStackBase::isNull
        static auto isNullFunc = reinterpret_cast<bool (*)(void* self)>(glacie::memory::resolveIdentifier(
            "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 83 EC ?? 80 79 ?? ?? 48 8B D9"
        ));

        if (auto* player = reinterpret_cast<void*>(getOwnerFunc(this)); player != nullptr) {
            auto* itemStack = getSelectedItemFunc(player);
            auto* item      = getItemFunc(itemStack);
            glacie::memory::virtualCall<void*, void*, void*>(item, 76, itemStack, player); // FishingRodItem::use
            if (!isNullFunc(itemStack)) {
                glacie::memory::virtualCall<void*, void*, void*>(item, 76, itemStack, player); // FishingRodItem::use
            }
            glacie::memory::virtualCall<void, bool>(player, 165, true); // Player::sendInventory
        }
    }
    return result;
}

namespace AutomaticFishing {

Entry* Entry::getInstance() {
    static Entry* instance;
    if (!instance) instance = new Entry();
    return instance;
}

void Entry::onEnable() { ServerHookedHook::hook(); }

void Entry::onDisable() { ServerHookedHook::unhook(); }

} // namespace AutomaticFishing

extern "C" [[maybe_unused]] __declspec(dllexport) endstone::Plugin* init_endstone_plugin() {
    return AutomaticFishing::Entry::getInstance();
}