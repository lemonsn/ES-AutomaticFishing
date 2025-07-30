#include "Entry.h"
#include <glacie/memory/Hook.h>

GLACIE_AUTO_INSTANCE_HOOK(
    ServerHookedHook,
    // FishingHook::_serverHooked
    "40 53 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 ?? 48 8B D9 48 8B 89 ?? ?? ?? ?? 48 8B 01 48 "
    "8B 80",
    bool
) {
    auto result = origin();
    if (result && !glacie::memory::dAccess<int>(this, 976)) { // FishingHook::mTimeUntilHooked
        // FishingHook::getOwner
        static auto getOwnerFunc = SIGNATURE_CALL("48 89 5C 24 10 57 48 83 EC 20 48 8B B9 D8 01 00 00", void*, void*);

        // Player::getSelectedItem
        static auto getSelectedItemFunc =
            SIGNATURE_CALL("48 8B 91 ?? ?? ?? ?? 80 BA ?? ?? ?? ?? ?? 75 ?? 48 8B 8A ?? ?? ?? ?? 8B 52 ?? 48 8B 01 48 8B 40 ?? 48 FF 25 ?? ?? ?? ?? 48 8D 05", void*, void*);

        // ItemStackBase::getItem
        static auto getItemFunc = SIGNATURE_CALL("48 8B 41 08 48 85 C0 74 04 48 8B 00 C3", void*, void*);

        // ItemStackBase::isNull
        static auto isNullFunc =
            SIGNATURE_CALL("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 83 EC ?? 80 79 ?? ?? 48 8B D9", bool, void*);

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

} // namespace AutomaticFishing

extern "C" [[maybe_unused]] __declspec(dllexport) endstone::Plugin* init_endstone_plugin() {
    return AutomaticFishing::Entry::getInstance();
}