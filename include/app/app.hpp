#pragma once

#include "app/glgeom.hpp"
#include "app/util.hpp"
#include "gtc/quaternion.hpp"

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

  gl::mat4 controller_transform[2]; // [Left, Right]
  gl::vec4 controller_velocity[2];
  gl::vec4 controller_buttons[2];

  GLfloat time;
  GLfloat time_delta;
  GLint frame;

  GLfloat _pad; // Required to make the struct size a multiple of 16 bytes.
};

class App {
  gl::ivec2 m_particle_framebuffer_resolution{ 128, 128 };

  std::unique_ptr<gl::Framebuffer> m_particle_fbs[2];

  gl::VertexBuffer m_fullscreen_triangle_vb;
  gl::VertexBuffer m_particles_vb;
  gl::VertexBuffer m_particle_quad_vb;

  CommonShaderUniforms m_common_uniforms;
  gl::UniformBuffer m_common_uniforms_buffer;

  gl::Program m_programs[2];

  gl::vec4 m_controller_position[2];
  gl::quat m_controller_orientation[2];

  std::string m_user_shader_sources[3];
  std::string m_user_shader_sources_concatenated[3];
  std::string_view m_user_shader_source_prefixes[3];
  std::string_view m_user_shader_source_postfixes[3];

  std::string_view m_common_uniforms_shader_source;
  std::string_view m_simulate_shader_vs_source;

  FrameClock m_clock;

  void updateViewAndProjectionTransforms();
  void updateControllerTransforms();

  std::string concatenateShaderSourceAtIndex(int index);

public:
  bool init();
  void cleanup();
  void update(int frame_id, double time_seconds, double time_delta_seconds);
  void render(int width, int height);

  std::string_view getUserShaderSourceAtIndex(int index);
  void setUserShaderSourceAtIndex(int index, std::string_view shader_src);
  void tryCompileShaderPrograms();

  void setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values);
  void setControllerAtIndex(int index, const float *position_values, const float *velocity_values, const float *orientation_values, const float *buttons_values);

  double getAverageFramesPerSecond() const;
};
