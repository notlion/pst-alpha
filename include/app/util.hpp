#pragma once

#include "glutil.hpp"
#include "platform.hpp"


// Random

void randSeed(uint32_t seed);

float randFloat();
float randFloat(float max);
float randFloat(float min, float max);

double randFloat64();
double randFloat64(double max);
double randFloat64(double min, double max);

int randInt(int max);
int randInt(int min, int max);

bool randChance(int denom);


// Math

template <typename T>
constexpr T pi() {
  return T(3.14159265359);
}

template <typename T>
constexpr T tau() {
  return T(6.28318530718);
}

template <typename T>
constexpr T radians(const T &degrees) {
  return degrees * (pi<T>() / T(180));
}

template <typename T>
constexpr T degrees(const T &radians) {
  return radians * (T(180) / pi<T>());
}

template <typename T>
constexpr T fract(const T &x) {
  return x - std::floor(x);
}

template <typename T>
constexpr gl::tvec2<T> floor(const gl::tvec2<T> &v) {
  return gl::tvec2<T>(std::floor(v.x), std::floor(v.y));
}

template <typename T>
constexpr gl::tvec3<T> floor(const gl::tvec3<T> &v) {
  return gl::tvec3<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
}

template <typename T>
constexpr gl::tvec4<T> floor(const gl::tvec4<T> &v) {
  return gl::tvec4<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w));
}

template <typename T>
constexpr gl::tvec2<T> fract(const gl::tvec2<T> &v) {
  return v - floor(v);
}

template <typename T>
constexpr gl::tvec3<T> fract(const gl::tvec3<T> &v) {
  return v - floor(v);
}

template <typename T>
constexpr gl::tvec4<T> fract(const gl::tvec4<T> &v) {
  return v - floor(v);
}

template <typename T>
constexpr T triangle(const T &x) {
  return std::abs(fract(x + T(0.5)) * T(2) - T(1));
}

template <typename T>
constexpr T clamp(const T &x, const T &min, const T &max) {
  return x < min ? min : x > max ? max : x;
}

template <typename T>
constexpr T saturate(const T &x) {
  return x < T(0) ? T(0) : x > T(1) ? T(1) : x;
}

template <typename T>
constexpr T mix(const T &a, const T &b, const T &t) {
  return t * (b - a) + a;
}

template <typename T>
constexpr gl::tvec2<T> mix(const gl::tvec2<T> &a, const gl::tvec2<T> &b, const T &t) {
  return t * (b - a) + a;
}

template <typename T>
constexpr gl::tvec3<T> mix(const gl::tvec3<T> &a, const gl::tvec3<T> &b, const T &t) {
  return t * (b - a) + a;
}

template <typename T>
constexpr gl::tvec4<T> mix(const gl::tvec4<T> &a, const gl::tvec4<T> &b, const T &t) {
  return t * (b - a) + a;
}

template <typename T>
constexpr T smoothstep(const T &edge0, const T &edge1, T x) {
  x = saturate((x - edge0) / (edge1 - edge0));
  return x * x * (T(3) - T(2) * x);
}

template <typename T>
constexpr T smootherstep(const T &edge0, const T &edge1, T x) {
  x = saturate((x - edge0) / (edge1 - edge0));
  return x * x * x * (x * (x * T(6) - T(15)) + T(10));
}

template <typename T>
constexpr T atan2(const gl::tvec2<T> &v) {
  return std::atan2(v.y, v.x);
}

template <typename T, size_t n>
constexpr size_t arraySize(const T (&)[n]) {
  return n;
}


// Color

constexpr uint32_t packColor32(const gl::vec4 &color);
constexpr uint32_t packColor24(const gl::vec3 &color);

gl::vec4 unpackColor32(uint32_t packed);
gl::vec3 unpackColor24(uint32_t packed);

gl::vec4 unpackColor32(const uint8_t *packed);
gl::vec3 unpackColor24(const uint8_t *packed);


// Strings

bool stringsEqualCaseInsensitive(std::string_view s1, std::string_view s2);


// Formatting

std::string formatString(const char *fmt, ...);


// Clock

class FrameClock {
  double m_start_time_seconds = 0.0;
  double m_time_seconds = 0.0;
  double m_average_fps_start_time_seconds = 0.0;

  double m_average_fps_accum = 0.0;
  int m_average_fps_count = 0;

  bool m_has_started = false;

public:
  double elapsed_seconds = 0.0;
  double elapsed_seconds_delta = 0.0;

  double average_fps = 0.0;

  uint32_t elapsed_frames = 0;

  void start(double time_seconds);
  void tick(double time_seconds);
};
