#pragma once

#include "app/glgeom.hpp"
#include "app/util.hpp"

#include <array>
#include <string>
#include <string_view>

struct Camera {
  gl::vec3 position = gl::vec3(0.0f);
  gl::vec3 target = gl::vec3(0.0f);
  gl::vec3 up = gl::vec3(0.0f, 1.0f, 0.0f);

  float fovy = 60.0f;
};

class App {
  GLuint m_particle_buffer_id;
  GLuint m_particle_vertex_array_id;
  GLint m_particle_framebuffer_resolution = 512;

  std::array<std::unique_ptr<gl::Framebuffer>, 3> m_particle_fbs;

  gl::VertexBuffer m_fullscreen_triangle_vb;
  gl::VertexBuffer m_particles_vb;

  gl::Program m_simulate_prog;
  gl::Program m_render_prog;

  FrameClock m_clock;

  gl::mat4 m_view_matrix;
  gl::mat4 m_projection_matrix;

  std::string_view m_shader_source_simulate_prefix;
  std::string_view m_shader_source_simulate_postfix;

  std::string m_user_shader_source;

public:
  bool init();
  void cleanup();
  void update(double time_seconds);
  void render(int width, int height);

  std::string_view getShaderSource();
  void setShaderSource(std::string_view shader_src);

  void setViewMatrix(const float *values);
  void setProjectionMatrix(const float *values);
};
