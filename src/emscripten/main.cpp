#include "app/app.hpp"
#include "app/log.hpp"

#include <emscripten.h>
#include <emscripten/html5.h>

static App g_app;

static const char *emscripten_result_to_string(EMSCRIPTEN_RESULT result) {
  if (result == EMSCRIPTEN_RESULT_SUCCESS) return "EMSCRIPTEN_RESULT_SUCCESS";
  if (result == EMSCRIPTEN_RESULT_DEFERRED) return "EMSCRIPTEN_RESULT_DEFERRED";
  if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED) return "EMSCRIPTEN_RESULT_NOT_SUPPORTED";
  if (result == EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED) return "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED";
  if (result == EMSCRIPTEN_RESULT_INVALID_TARGET) return "EMSCRIPTEN_RESULT_INVALID_TARGET";
  if (result == EMSCRIPTEN_RESULT_UNKNOWN_TARGET) return "EMSCRIPTEN_RESULT_UNKNOWN_TARGET";
  if (result == EMSCRIPTEN_RESULT_INVALID_PARAM) return "EMSCRIPTEN_RESULT_INVALID_PARAM";
  if (result == EMSCRIPTEN_RESULT_FAILED) return "EMSCRIPTEN_RESULT_FAILED";
  if (result == EMSCRIPTEN_RESULT_NO_DATA) return "EMSCRIPTEN_RESULT_NO_DATA";
  return "Unknown EMSCRIPTEN_RESULT!";
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void init() {
  g_app.init();
}

EMSCRIPTEN_KEEPALIVE
void update(double time_seconds) {
  g_app.update(time_seconds);
}

EMSCRIPTEN_KEEPALIVE
void render(double width, double height) {
  g_app.render(int(width), int(height));
}

EMSCRIPTEN_KEEPALIVE
const char *getSimulationShaderSource() {
  auto src = g_app.getSimulationShaderSource();
  return src.data();
}

EMSCRIPTEN_KEEPALIVE
void setSimulationShaderSource(const char *shader_src) {
  return g_app.setSimulationShaderSource(shader_src);
}

EMSCRIPTEN_KEEPALIVE
void setViewMatrix(const float *values) {
  g_app.setViewMatrix(values);
}

EMSCRIPTEN_KEEPALIVE
void setProjectionMatrix(const float *values) {
  g_app.setProjectionMatrix(values);
}

}
