#pragma once

#if _WIN32 || _WIN64
#define PLATFORM_WINDOWS
#else
#if __linux__
#define PLATFORM_LINUX
#elif __APPLE__
#define PLATFORM_MAC
#else
#error "Unable to determine platform!"
#endif
#endif /* _WIN32 || _WIN64 */

#ifdef PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#define FORCENOINLINE _declspec(noinline)
#define ALIGN(n) __declspec(align(n))
#define WIN32_LEAN_AND_MEAN

#ifdef ONYX_BUILD_LIB
#define ONYX_API __declspec(dllexport)
#define ONYX_EXPORT __declspec(dllexport)
#else
#define ONYX_API __declspec(dllimport)
#define ONYX_EXPORT
#endif /* ONYX_BUILD_LIB */

#elif PLATFORM_LINUX || PLATFORM_MAC
#define FORCEINLINE inline
#error "Non-Windows platforms are not yet supported!"
#endif /* PLATFORM_WINDOWS */

#define ONYX_ASSERT_ENABLE
#ifdef ONYX_ASSERT_ENABLE
#include <iostream>

#if _MSC_VER
#include <intrin.h>
#define OnyxDebugBreak() __debugbreak();
#else
#define OnyxDebugBreak() __asm { int 3}
#endif /* _MSC_VER */

#define ASSERT(expr)                                         \
  {                                                          \
    if (expr) {                                              \
    } else {                                                 \
      ReportAssertionFailure(#expr, "", __FILE__, __LINE__); \
      OnyxDebugBreak();                                          \
    }                                                        \
  }

#define ASSERT_MSG(expr, message)                                 \
  {                                                               \
    if (expr) {                                                   \
    } else {                                                      \
      ReportAssertionFailure(#expr, message, __FILE__, __LINE__); \
      OnyxDebugBreak();                                               \
    }                                                             \
  }

#ifdef _DEBUG
#define ASSERT_DEBUG(expr)                                   \
  {                                                          \
    if (expr) {                                              \
    } else {                                                 \
      ReportAssertionFailure(#expr, "", __FILE__, __LINE__); \
      OnyxDebugBreak();                                          \
    }                                                        \
  }
#else
#define ASSERT_DEBUG(expr)
#endif

FORCEINLINE void ReportAssertionFailure(const char *expression,
                                        const char *message, const char *file,
                                        int line) {
  std::cerr << "Assertion Failure: " << expression << "\n";
  std::cerr << "  Message: " << message << "\n";
  std::cerr << "  At: " << file << ":" << line << "\n";
}

#else
#define ASSERT(expr)
#define ASSERT_MSG(expr, message)
#define ASSERT_DEBUG(expr)
#endif