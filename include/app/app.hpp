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
  gl::ivec2 m_particle_framebuffer_resolution{ 128, 128 };

  std::array<std::unique_ptr<gl::Framebuffer>, 3> m_particle_fbs;

  gl::VertexBuffer m_fullscreen_triangle_vb;
  gl::VertexBuffer m_particles_vb;
  gl::VertexBuffer m_particle_quad_vb;

  gl::Program m_simulate_prog;
  gl::Program m_texture_prog;

  FrameClock m_clock;

  gl::mat4 m_view_matrix;
  gl::mat4 m_projection_matrix;
  gl::mat4 m_view_projection_matrix;
  gl::mat4 m_inverse_view_matrix;
  gl::mat4 m_inverse_projection_matrix;
  gl::mat4 m_inverse_view_projection_matrix;

  std::string m_user_shader_sources[2];
  std::string_view m_user_shader_source_prefixes[2];
  std::string_view m_user_shader_source_postfixes[2];

  void setCommonShaderUniforms(gl::Program &prog);

public:
  bool init();
  void cleanup();
  void update(double time_seconds);
  void render(int width, int height);

  std::string_view getUserShaderSourceAtIndex(int index);
  void setUserShaderSourceAtIndex(std::string_view shader_src, int index);

  void setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values);

  double getAverageFramesPerSecond() const;
};
