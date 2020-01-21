#include "app/log.hpp"

#if defined(PLATFORM_ANDROID)

#elif defined(PLATFORM_WINDOWS)

#include <Windows.h>
#include <cstdio>
#include <cstdarg>
#include <vector>

void platform_windows_print(const char *format, ...) {
  va_list args;
  va_start(args, format);
  std::vector<char> buffer(1 + std::vsnprintf(nullptr, 0, fmt, args));
  std::vsnprintf(buffer.data(), buffer.size(), format, args);
  va_end(args);

  OutputDebugStringA(buffer.data());
}

#else

#include <cstdio>
#include <cstdarg>

void platform_posix_print(const char *format, ...) {
  va_list args;
  va_start(args, format);
  std::vprintf(format, args);
  va_end(args);
}

void platform_posix_print_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  std::vfprintf(stderr, format, args);
  va_end(args);
}

#endif
