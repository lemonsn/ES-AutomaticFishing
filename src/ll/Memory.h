/**
此文件来自于https://github.com/LiteLDev/LeviLamina/blob/main/src/ll/api/memory/Memory.h
*/

#pragma once
#include <cstdint>
#include <utility>

namespace ll::memory {

template <class T>
[[nodiscard]] constexpr T& dAccess(void* ptr, ptrdiff_t off) {
    return *(T*)((uintptr_t)((uintptr_t)ptr + off));
}

template <class T>
[[nodiscard]] constexpr T const& dAccess(void const* ptr, ptrdiff_t off) {
    return *(T*)((uintptr_t)((uintptr_t)ptr + off));
}

template <class R = void, class... Args>
constexpr auto virtualCall(void const* self, ptrdiff_t vIndex, auto&&... args) -> R {
    return (*(R(**)(void const*, Args...))(*(uintptr_t**)self + vIndex))(self, std::forward<decltype((args))>(args)...);
}

} // namespace ll::memory