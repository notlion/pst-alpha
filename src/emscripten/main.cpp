#include "app/app.hpp"
#include "app/log.hpp"

#include <emscripten.h>
#include <emscripten/html5.h>

static App g_app;

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
void render(int width, int height) {
  g_app.render(width, height);
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
void setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values) {
  g_app.setViewAndProjectionMatrices(view_matrix_values, projection_matrix_values);
}

EMSCRIPTEN_KEEPALIVE
double getAverageFramesPerSecond() {
  return g_app.getAverageFramesPerSecond();
}

} // extern "C"
