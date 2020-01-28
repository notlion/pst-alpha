#pragma once

#include "app/glgeom.hpp"
#include "app/util.hpp"
#include "gtc/quaternion.hpp"

#include <array>
#include <string>
#include <string_view>

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

  gl::ivec2 size;

  GLfloat time;
  GLfloat time_delta;
  GLint frame;

  GLfloat _pad[3]; // Required to make the struct size a multiple of 16 bytes.
};

class App {
  static constexpr size_t USER_SHADER_SOURCE_COUNT{ 4 };
  static constexpr size_t ASSEMBLED_SHADER_SOURCE_COUNT{ 3 };
  static constexpr size_t TEMPLATE_SHADER_SOURCE_COUNT{ 3 };

  gl::ivec2 m_default_particle_framebuffer_resolution{ 128, 128 };
  gl::ivec2 m_particle_framebuffer_resolution = m_default_particle_framebuffer_resolution;

  std::unique_ptr<gl::Framebuffer> m_particle_fbs[2];

  gl::VertexBuffer m_fullscreen_triangle_vb;

  GLsizei m_default_instance_vertex_count{ 6 };
  GLsizei m_instance_vertex_count = m_default_instance_vertex_count;

  GLenum m_default_cull_mode{ GL_NONE };
  GLenum m_cull_mode = m_default_cull_mode;

  GLenum m_default_blend_func_sfactor{ GL_ZERO };
  GLenum m_default_blend_func_dfactor{ GL_ZERO };
  GLenum m_blend_func_sfactor = m_default_blend_func_sfactor;
  GLenum m_blend_func_dfactor = m_default_blend_func_dfactor;

  GLenum m_default_depth_func{ GL_LESS };
  GLenum m_depth_func = m_default_depth_func;

  CommonShaderUniforms m_common_uniforms;
  gl::UniformBuffer m_common_uniforms_buffer;

  gl::Program m_programs[2];

  gl::vec4 m_controller_position[2];
  gl::quat m_controller_orientation[2];

  std::string m_user_shader_sources[USER_SHADER_SOURCE_COUNT];
  std::string m_assembled_shader_sources[ASSEMBLED_SHADER_SOURCE_COUNT];

  std::string_view m_template_shader_source_prefixes[TEMPLATE_SHADER_SOURCE_COUNT];
  std::string_view m_template_shader_source_postfixes[TEMPLATE_SHADER_SOURCE_COUNT];

  std::string_view m_common_uniforms_shader_source;
  std::string_view m_simulate_shader_vs_source;

  FrameClock m_clock;

  void updateViewAndProjectionTransforms();
  void updateControllerTransforms();

  std::string assembleShaderSourceAtIndex(int index);

  void parseSimulationShaderPragmas();
  void parseRenderShaderPragmas();

public:
  bool init();
  void cleanup();
  void update(int frame_id, double time_seconds, double time_delta_seconds);
  void simulate(int displayWidth, int displayHeight);
  void render(int displayWidth, int displayHeight);

  std::string_view getUserShaderSourceAtIndex(int index);
  std::string_view getAssembledShaderSourceAtIndex(int index);
  void setUserShaderSourceAtIndex(int index, std::string_view shader_src);

  bool tryCompileShaderPrograms();

  void setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values);
  void setControllerAtIndex(int index, const float *position_values, const float *velocity_values, const float *orientation_values, const float *buttons_values);

  double getAverageFramesPerSecond() const;
};
