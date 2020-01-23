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
void update(int frame_id, double time_seconds, double time_delta_seconds) {
  g_app.update(frame_id, time_seconds, time_delta_seconds);
}

EMSCRIPTEN_KEEPALIVE
void simulate(int displayWidth, int displayHeight) {
  g_app.simulate(displayWidth, displayHeight);
}

EMSCRIPTEN_KEEPALIVE
void render(int displayWidth, int displayHeight) {
  g_app.render(displayWidth, displayHeight);
}


EMSCRIPTEN_KEEPALIVE
const char *getAssembledShaderSourceAtIndex(int index) {
  auto src = g_app.getAssembledShaderSourceAtIndex(index);
  return src.data();
}

EMSCRIPTEN_KEEPALIVE
const char *getUserShaderSourceAtIndex(int index) {
  auto src = g_app.getUserShaderSourceAtIndex(index);
  return src.data();
}

EMSCRIPTEN_KEEPALIVE
void setUserShaderSourceAtIndex(int index, const char *shader_src) {
  g_app.setUserShaderSourceAtIndex(index, shader_src);
}

EMSCRIPTEN_KEEPALIVE
bool tryCompileShaderPrograms() {
  return g_app.tryCompileShaderPrograms();
}

EMSCRIPTEN_KEEPALIVE
void setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values) {
  g_app.setViewAndProjectionMatrices(view_matrix_values, projection_matrix_values);
}

EMSCRIPTEN_KEEPALIVE
void setControllerAtIndex(int index, const float *position_values, const float *velocity_values, const float *orientation_values, const float *buttons_values) {
  g_app.setControllerAtIndex(index, position_values, velocity_values, orientation_values, buttons_values);
}

EMSCRIPTEN_KEEPALIVE
double getAverageFramesPerSecond() {
  return g_app.getAverageFramesPerSecond();
}

} // extern "C"
