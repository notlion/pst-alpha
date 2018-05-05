#pragma once

#if defined(_WIN32)
  #define PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define PLATFORM_APPLE
  #include "TargetConditionals.h"
  #if TARGET_OS_IOS
    #define PLATFORM_IOS
  #elif TARGET_OS_OSX
    #define PLATFORM_OSX
  #endif
#elif defined(__ANDROID__)
  #define PLATFORM_ANDROID
#elif defined(__EMSCRIPTEN__)
  #define PLATFORM_EMSCRIPTEN
#endif
