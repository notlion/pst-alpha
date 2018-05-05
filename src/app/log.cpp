#include "app/log.hpp"

#if defined(PLATFORM_WINDOWS)

#include <Windows.h>
#include <cstdio>
#include <vector>

void platform_windows_printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  std::vector<char> buffer(1 + std::vsnprintf(nullptr, 0, fmt, args));
  std::vsnprintf(buffer.data(), buffer.size(), fmt, args);
  va_end(args);

  OutputDebugStringA(buffer.data());
}

#endif
