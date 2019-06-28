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
const char *getUserShaderSourceAtIndex(int index) {
  auto src = g_app.getUserShaderSourceAtIndex(index);
  return src.data();
}

EMSCRIPTEN_KEEPALIVE
void setUserShaderSourceAtIndex(const char *shader_src, int index) {
  return g_app.setUserShaderSourceAtIndex(shader_src, index);
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
