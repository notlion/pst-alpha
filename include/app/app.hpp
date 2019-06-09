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
  GLint m_particle_framebuffer_resolution = 512;

  std::array<std::unique_ptr<gl::Framebuffer>, 3> m_particle_fbs;

  gl::VertexBuffer m_fullscreen_triangle_vb;
  gl::VertexBuffer m_particles_vb;

  gl::Program m_simulate_prog;
  gl::Program m_texture_prog;

  FrameClock m_clock;

  gl::mat4 m_view_matrix;
  gl::mat4 m_projection_matrix;
  gl::mat4 m_view_projection_matrix;
  gl::mat4 m_inverse_view_matrix;
  gl::mat4 m_inverse_projection_matrix;
  gl::mat4 m_inverse_view_projection_matrix;

  std::string_view m_simulation_shader_source_prefix;
  std::string_view m_simulation_shader_source_postfix;
  std::string m_user_simulation_shader_source;

  std::string_view m_texture_shader_source_prefix;
  std::string_view m_texture_shader_source_postfix;
  std::string m_user_texture_shader_source;

  void updateViewProjectionMatrices();
  void setCommonShaderUniforms(gl::Program &prog);

public:
  bool init();
  void cleanup();
  void update(double time_seconds);
  void render(int width, int height);

  std::string_view getSimulationShaderSource();
  void setSimulationShaderSource(std::string_view shader_src);

  std::string_view getTextureShaderSource();
  void setTextureShaderSource(std::string_view shader_src);

  void setViewMatrix(const float *values);
  void setProjectionMatrix(const float *values);
};
