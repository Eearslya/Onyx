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