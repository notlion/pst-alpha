#pragma once

#include "platform.hpp"

#if defined(PLATFORM_ANDROID)

#include <android/log.h>

#define PRINT_INFO(...) __android_log_print(ANDROID_LOG_INFO, "Visualization", __VA_ARGS__)
#define PRINT_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "Visualization", __VA_ARGS__)
#define PRINT_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "Visualization", __VA_ARGS__)

#elif defined(PLATFORM_WINDOWS)

void platform_windows_printf(const char *fmt, ...);

#define PRINT_INFO(...) platform_windows_printf(__VA_ARGS__)
#define PRINT_ERROR(...) platform_windows_printf(__VA_ARGS__)

#if defined(NDEBUG)
  #define PRINT_DEBUG(...)
#else
  #define PRINT_DEBUG(...) platform_windows_printf(__VA_ARGS__)
#endif

#else

#include <cstdio>

#define PRINT_INFO(...) std::printf(__VA_ARGS__)
#define PRINT_ERROR(...) std::fprintf(stderr, __VA_ARGS__)

#if defined(NDEBUG)
  #define PRINT_DEBUG(...)
#else
  #define PRINT_DEBUG(...) std::printf(__VA_ARGS__)
#endif

#endif
