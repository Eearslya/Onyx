#pragma once

// Determine our build platform
#if _WIN32 || _WIN64
#define ONYX_PLATFORM_WINDOWS
#elif defined(__linux__)
#define ONYX_PLATFORM_LINUX
#elif defined(__APPLE__)
#define ONYX_PLATFORM_APPLE
#else
#error "Unsupported or unknown build platform!"
#endif

// Determine our debug level
//#define ONYX_TRACE

#ifdef _DEBUG
#define ONYX_DEBUG
#endif

// Set up our platform-specific macros
#ifdef ONYX_PLATFORM_WINDOWS
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
#endif

#else
#error "Non-Windows platforms are not yet supported!"
#endif

// Set up assertions
#ifndef ONYX_NO_ASSERT
#define ONYX_ASSERT_ENABLE
#endif

#ifdef ONYX_ASSERT_ENABLE
#include <iostream>

// Set our debug break handler
#if _MSC_VER
#include <intrin.h>
#define OnyxDebugBreak() __debugbreak();
#else
#define OnyxDebugBreak() __asm {int 3}
#endif

#define ASSERT_MSG(expr, msg)                               \
  {                                                         \
    if (expr) {                                             \
    } else {                                                \
      OnyxAssertionFailure(#expr, msg, __FILE__, __LINE__); \
      OnyxDebugBreak();                                     \
    }                                                       \
  }

#define ASSERT(expr) ASSERT_MSG(expr, "")

#ifdef ONYX_DEBUG
#define ASSERT_DEBUG(expr) ASSERT(expr)
#define ASSERT_DEBUG_MSG(expr, msg) ASSERT_MSG(expr, msg)
#else
#define ASSERT_DEBUG(expr)
#define ASSERT_DEBUG_MSG(expr, msg)
#endif

FORCEINLINE void OnyxAssertionFailure(const char* expression, const char* msg,
                                      const char* file, int line) {
  std::cerr << "Assertion Failure: " << expression << "\n";
  std::cerr << "  Message: " << msg << "\n";
  std::cerr << "  At: " << file << ":" << line << "\n";
}

#else
#define ASSERT(expr)
#define ASSERT_MSG(expr, msg)
#define ASSERT_DEBUG(expr)
#define ASSERT_DEBUG_MSG(expr, msg)
#endif