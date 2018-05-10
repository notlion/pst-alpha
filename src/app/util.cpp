#include "app/util.hpp"
#include "app/log.hpp"
#include "app/platform.hpp"

#include <algorithm>
#include <random>


static std::mt19937 g_random_engine;

void randSeed(uint32_t seed) {
  g_random_engine.seed(seed);
}

float randFloat() {
  return std::uniform_real_distribution<float>()(g_random_engine);
}

float randFloat(float max) {
  return randFloat(0.0f, max);
}

float randFloat(float min, float max) {
  return std::uniform_real_distribution<float>(std::min(min, max),
                                               std::max(min, max))(g_random_engine);
}

double randFloat64() {
  return std::uniform_real_distribution<double>()(g_random_engine);
}

double randFloat64(double max) {
  return randFloat64(0.0, max);
}

double randFloat64(double min, double max) {
  return std::uniform_real_distribution<double>(std::min(min, max),
                                                std::max(min, max))(g_random_engine);
}

int randInt(int max) {
  return randInt(0, max);
}

int randInt(int min, int max) {
  return std::uniform_int_distribution<int>(min, max)(g_random_engine);
}

bool randChance(int denom) {
  return randInt(1, denom) == 1;
}


constexpr uint32_t packColor32(const gl::vec4 &color) {
  return uint32_t(color.a * 255.0f) << 24
       | uint32_t(color.r * 255.0f) << 16
       | uint32_t(color.g * 255.0f) << 8
       | uint32_t(color.b * 255.0f);
}

constexpr uint32_t packColor24(const gl::vec3 &color) {
  return 0xff000000 // NOTE(ryan): Set alpha to opaque in case this is passed to the 32-bit unpack function.
       | uint32_t(color.r * 255.0f) << 16
       | uint32_t(color.g * 255.0f) << 8
       | uint32_t(color.b * 255.0f);
}

gl::vec4 unpackColor32(uint32_t packed) {
  const auto a = (packed >> 24) & 255;
  const auto r = (packed >> 16) & 255;
  const auto g = (packed >> 8) & 255;
  const auto b = packed & 255;
  return gl::vec4(r, g, b, a) * (1.0f / 255.0f);
}

gl::vec3 unpackColor24(uint32_t packed) {
  const auto r = (packed >> 16) & 255;
  const auto g = (packed >> 8) & 255;
  const auto b = packed & 255;
  return gl::vec3(r, g, b) * (1.0f / 255.0f);
}

gl::vec4 unpackColor32(const uint8_t *packed) {
  return gl::vec4(packed[0], packed[1], packed[2], packed[3]) * (1.0f / 255.0f);
}

gl::vec3 unpackColor24(const uint8_t *packed) {
  return gl::vec3(packed[0], packed[1], packed[2]) * (1.0f / 255.0f);
}


std::string formatString(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  std::vector<char> buf(1 + std::vsnprintf(nullptr, 0, fmt, args), '\0');
  std::vsnprintf(buf.data(), buf.size(), fmt, args);
  va_end(args);
  return { buf.begin(), buf.end() };
}


void FrameClock::start(double time_seconds) {
  m_start_time_seconds = m_time_seconds = time_seconds;
  m_average_fps_start_time_seconds = m_time_seconds;
  m_has_started = true;
}

void FrameClock::tick(double time_seconds) {
  if (!m_has_started) {
    start(time_seconds);
  }
  else {
    elapsed_frames++;
  }

  elapsed_seconds_delta = time_seconds - m_time_seconds;
  elapsed_seconds = time_seconds - m_start_time_seconds;
  
  m_time_seconds = time_seconds;

  m_average_fps_accum += 1.0 / elapsed_seconds_delta;
  m_average_fps_count += 1;
  if (time_seconds - m_average_fps_start_time_seconds > 1.0) {
    average_fps = m_average_fps_accum / m_average_fps_count;

    m_average_fps_accum = 0.0;
    m_average_fps_count = 0;
    m_average_fps_start_time_seconds = time_seconds;
  }
}
