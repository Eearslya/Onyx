#pragma once

#ifdef _WIN32
#define ONYX_PLATFORM_WINDOWS
#endif

#ifdef ONYX_PLATFORM_WINDOWS
#ifdef ONYX_BUILD_DLL
#define ONYX_API __declspec(dllexport)
#else
#define ONYX_API __declspec(dllimport)
#endif
#else
#error Unsupported platform!
#endif

#ifndef IMGUI_API
#define IMGUI_API ONYX_API
#endif

#include <memory>

namespace Onyx {
template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}
}  // namespace Onyx