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

struct CommonShaderUniforms {
  gl::mat4 model_view_projection;
  gl::mat4 model_view;
  gl::mat4 projection;
  gl::mat4 inverse_model_view_projection;
  gl::mat4 inverse_model_view;
  gl::mat4 inverse_projection;

  gl::vec4 controller_position[2]; // [Left, Right]
  gl::vec4 controller_velocity[2];

  float time;
  float time_delta;
  float frame;

  float _pad; // Required to make the struct size a multiple of 16 bytes.
};

class App {
  gl::ivec2 m_particle_framebuffer_resolution{ 128, 128 };

  std::array<std::unique_ptr<gl::Framebuffer>, 3> m_particle_fbs;

  gl::VertexBuffer m_fullscreen_triangle_vb;
  gl::VertexBuffer m_particles_vb;
  gl::VertexBuffer m_particle_quad_vb;

  CommonShaderUniforms m_common_uniforms;
  gl::UniformBuffer m_common_uniforms_buffer;

  gl::Program m_simulate_prog;
  gl::Program m_texture_prog;

  FrameClock m_clock;

  std::string m_user_shader_sources[2];
  std::string_view m_user_shader_source_prefixes[2];
  std::string_view m_user_shader_source_postfixes[2];

  void bindCommonShaderUniforms(gl::Program &prog);
  void updateCommonShaderUniformMatrices();

public:
  bool init();
  void cleanup();
  void update(double time_seconds);
  void render(int width, int height);

  std::string_view getUserShaderSourceAtIndex(int index);
  void setUserShaderSourceAtIndex(int index, std::string_view shader_src);

  void setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values);
  void setControllerPoseAtIndex(int index, const float *position_values, const float *velocity_values);

  double getAverageFramesPerSecond() const;
};
