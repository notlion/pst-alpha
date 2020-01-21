#pragma once

#include "platform.hpp"

#if defined(PLATFORM_ANDROID)

#include <android/log.h>

#define PRINT_INFO(...) __android_log_print(ANDROID_LOG_INFO, "PST", __VA_ARGS__)
#define PRINT_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "PST", __VA_ARGS__)
#define PRINT_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "PST", __VA_ARGS__)

#elif defined(PLATFORM_WINDOWS)

void platform_windows_print(const char *format, ...);

#define PRINT_INFO(...) platform_windows_print(__VA_ARGS__)
#define PRINT_ERROR(...) platform_windows_print(__VA_ARGS__)

#if defined(NDEBUG)
  #define PRINT_DEBUG(...)
#else
  #define PRINT_DEBUG(...) platform_windows_print(__VA_ARGS__)
#endif

#else

void platform_posix_print(const char *format, ...);
void platform_posix_print_error(const char *format, ...);

#define PRINT_INFO(...) platform_posix_print(__VA_ARGS__)
#define PRINT_ERROR(...) platform_posix_print_error(__VA_ARGS__)

#if defined(NDEBUG)
  #define PRINT_DEBUG(...)
#else
  #define PRINT_DEBUG(...) platform_posix_print(__VA_ARGS__)
#endif

#endif
