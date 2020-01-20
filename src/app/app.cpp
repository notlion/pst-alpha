#include "app/app.hpp"

#include "app/log.hpp"
#include "app/shaders.hpp"
#include "app/util.hpp"

#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"

using namespace std::string_literals;

struct PositionVertex {
  gl::vec4 position;
};

struct PositionTexcoord2DVertex {
  gl::vec2 position;
  gl::vec2 texcoord;
};

static void splitShaderSource(std::string_view source,
                              std::string_view line_marker,
                              std::string_view &out_prefix,
                              std::string_view &out_postfix) {
  const auto pos = source.find(line_marker) + line_marker.length();
  out_prefix = source.substr(0, pos);
  out_postfix = source.substr(pos, source.length() - pos);
}

bool App::init() {
  DEBUG_PRINT_GL_STATS();

  m_common_uniforms_shader_source = shader_source_common_uniforms;
  m_simulate_shader_vs_source = shader_source_simulation_vs;

  splitShaderSource(shader_source_simulation_fs, "{{simulation}}", m_template_shader_source_prefixes[0], m_template_shader_source_postfixes[0]);
  splitShaderSource(shader_source_shade_vs, "{{vertex}}", m_template_shader_source_prefixes[1], m_template_shader_source_postfixes[1]);
  splitShaderSource(shader_source_shade_fs, "{{fragment}}", m_template_shader_source_prefixes[2], m_template_shader_source_postfixes[2]);

  setUserShaderSourceAtIndex(0, shader_source_user_default_common);
  setUserShaderSourceAtIndex(1, shader_source_user_default_simulation);
  setUserShaderSourceAtIndex(2, shader_source_user_default_vertex);
  setUserShaderSourceAtIndex(3, shader_source_user_default_fragment);

  tryCompileShaderPrograms();

  // Create a triangle for rendering fullscreen
  {
    const PositionVertex vs[]{
      { gl::vec4(-1.0f, -1.0f, 0.0f, 1.0f) },
      { gl::vec4(3.0f, -1.0f, 0.0f, 1.0f) },
      { gl::vec4(-1.0f, 3.0f, 0.0f, 1.0f) },
    };

    gl::TriangleMesh<PositionVertex> fullscreen_triangle_mesh{
      {
        { { vs[0], vs[1], vs[2] } },
      },
      {
        { GL_FLOAT, 4, 0, 0, 0 },
      }
    };

    gl::createVertexBuffer(m_fullscreen_triangle_vb, fullscreen_triangle_mesh);
  }

  // Create particle vertex buffer
  {
    const auto vertex_count = m_particle_framebuffer_resolution.x * m_particle_framebuffer_resolution.y;

    std::vector<gl::ivec2> vertices;
    vertices.reserve(vertex_count);
    for (GLint y = 0; y < m_particle_framebuffer_resolution.y; ++y) {
      for (GLint x = 0; x < m_particle_framebuffer_resolution.x; ++x) {
        vertices.emplace_back(x, y);
      }
    }

    const std::vector<gl::VertexAttribute> attribs{
      { GL_INT, 2, 0, 0, 2 },
    };

    gl::createVertexBuffer(m_particles_vb, GL_POINTS, sizeof(gl::ivec2) * vertices.size(), vertices.size(), vertices.data(), GL_STATIC_DRAW, attribs);
  }

  // Create particle quad vertex buffer
  {
    const PositionTexcoord2DVertex vs[]{
      { gl::vec2(-1.0f, -1.0f), gl::vec2(0.0f, 0.0f) },
      { gl::vec2(-1.0f, 1.0f), gl::vec2(0.0f, 1.0f) },
      { gl::vec2(1.0f, -1.0f), gl::vec2(1.0f, 0.0f) },
      { gl::vec2(1.0f, 1.0f), gl::vec2(1.0f, 1.0f) },
    };

    gl::TriangleMesh<PositionTexcoord2DVertex> particle_quad_mesh{
      {
        { { vs[0], vs[1], vs[2] } },
        { { vs[1], vs[3], vs[2] } },
      },
      {
        { GL_FLOAT, 2, sizeof(PositionTexcoord2DVertex), offsetof(PositionTexcoord2DVertex, position), 0 },
        { GL_FLOAT, 2, sizeof(PositionTexcoord2DVertex), offsetof(PositionTexcoord2DVertex, texcoord), 1 },
      }
    };
    
    gl::createVertexBuffer(m_particle_quad_vb, particle_quad_mesh, GL_STATIC_DRAW);
  }

  // Create particle data framebuffers
  {
    gl::TextureOpts particle_tex_opts{ GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST };

    for (size_t i = 0; i < arraySize(m_particle_fbs); ++i) {
      m_particle_fbs[i] = std::make_unique<gl::Framebuffer>();
      gl::createFramebuffer(*m_particle_fbs[i],
                            m_particle_framebuffer_resolution.x,
                            m_particle_framebuffer_resolution.y,
                            {
                              { GL_COLOR_ATTACHMENT0, particle_tex_opts },
                              { GL_COLOR_ATTACHMENT1, particle_tex_opts },
                              { GL_COLOR_ATTACHMENT2, particle_tex_opts },
                              { GL_COLOR_ATTACHMENT3, particle_tex_opts },
                              { GL_COLOR_ATTACHMENT4, particle_tex_opts },
                              { GL_COLOR_ATTACHMENT5, particle_tex_opts },
                            });
    }
  }

  // Init shader uniforms
  {
    m_common_uniforms.model_view = glm::lookAt(gl::vec3(0.0f, 0.0f, 3.0f), gl::vec3(0.0f), gl::vec3(0.0f, 1.0f, 0.0f));
    m_common_uniforms.projection = glm::perspective(radians(60.0f), 1.0f, 0.01f, 1000.0f);

    updateViewAndProjectionTransforms();

    m_controller_position[0] = gl::vec4(-0.5f, 1.0f, 0.0f, 1.0f);
    m_controller_position[1] = gl::vec4(0.5f, 1.0f, 0.0f, 1.0f);

    gl::createUniformBuffer(m_common_uniforms_buffer, m_common_uniforms, GL_DYNAMIC_DRAW);
  }

  return true;
}

void App::cleanup() {
}

void App::updateViewAndProjectionTransforms() {
  m_common_uniforms.inverse_model_view = gl::inverse(m_common_uniforms.model_view);
  m_common_uniforms.inverse_projection = gl::inverse(m_common_uniforms.projection);
  m_common_uniforms.model_view_projection = m_common_uniforms.projection * m_common_uniforms.model_view;
  m_common_uniforms.inverse_model_view_projection = gl::inverse(m_common_uniforms.model_view_projection);
}

void App::updateControllerTransforms() {
  for (size_t i = 0; i < arraySize(m_common_uniforms.controller_transform); ++i) {
    m_common_uniforms.controller_transform[i] = gl::translate(gl::mat4(1.0f), gl::vec3(m_controller_position[i])) * gl::mat4_cast(m_controller_orientation[i]);
  }
}

void App::update(int frame_id, double time_seconds, double time_delta_seconds) {
  m_clock.tick(time_seconds);

  // Update common uniforms
  {
    updateControllerTransforms();

    m_common_uniforms.time = float(time_seconds);
    m_common_uniforms.time_delta = float(time_delta_seconds);
    m_common_uniforms.frame = frame_id;

    gl::updateUniformBuffer(m_common_uniforms_buffer, m_common_uniforms);
  }

  // Simulate
  {
    std::swap(m_particle_fbs[0], m_particle_fbs[1]);

    gl::bindFramebuffer(*m_particle_fbs[0]);

    gl::disableBlend();
    gl::disableDepth();

    glViewport(0, 0, m_particle_framebuffer_resolution.x, m_particle_framebuffer_resolution.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gl::bindTexture(m_particle_fbs[1]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[1]->textures[1], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[1]->textures[2], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[1]->textures[3], GL_TEXTURE3);
    gl::bindTexture(m_particle_fbs[1]->textures[4], GL_TEXTURE4);
    gl::bindTexture(m_particle_fbs[1]->textures[5], GL_TEXTURE5);

    gl::bindUniformBuffer(m_common_uniforms_buffer, 0);

    gl::useProgram(m_programs[0]);
    gl::uniform(m_programs[0], "iResolution", m_particle_framebuffer_resolution);

    gl::drawVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();

    CHECK_GL_ERROR();
  }
}

void App::render(int width, int height) {
  gl::updateUniformBuffer(m_common_uniforms_buffer, m_common_uniforms);

  // Texture
  {
    // gl::enableBlendAlphaPremult();
    gl::disableBlend();
    gl::enableDepth();

    gl::bindTexture(m_particle_fbs[0]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[0]->textures[1], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[0]->textures[2], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[0]->textures[3], GL_TEXTURE3);
    gl::bindTexture(m_particle_fbs[0]->textures[4], GL_TEXTURE4);
    gl::bindTexture(m_particle_fbs[0]->textures[5], GL_TEXTURE5);

    gl::bindUniformBuffer(m_common_uniforms_buffer, 0);

    gl::useProgram(m_programs[1]);
    gl::uniform(m_programs[1], "iResolution", gl::ivec2(width, height));

    gl::enableVertexBuffer(m_particle_quad_vb);
    gl::enableVertexBuffer(m_particles_vb);

    glVertexAttribDivisor(m_particle_quad_vb.attribs[0].loc, 0);
    glVertexAttribDivisor(m_particle_quad_vb.attribs[1].loc, 0);
    glVertexAttribDivisor(m_particles_vb.attribs[0].loc, 1);

    glDrawArraysInstanced(GL_TRIANGLES, 0, m_particle_quad_vb.count, m_particles_vb.count);

    glVertexAttribDivisor(m_particles_vb.attribs[0].loc, 0);

    gl::disableVertexBuffer(m_particles_vb);
    gl::disableVertexBuffer(m_particle_quad_vb);

    CHECK_GL_ERROR();
  }
}

static std::string concatenateShaderSource(std::string_view prefix,
                                           std::string_view common_source,
                                           std::string_view user_common_source,
                                           std::string_view user_source,
                                           std::string_view postfix) {
  auto src = std::string();
  auto src_size = prefix.size() + common_source.size() + user_common_source.size() + user_source.size() + postfix.size() + 3;
  src.reserve(src_size + 1);
  src += prefix;
  src += '\n';
  src += common_source;
  src += '\n';
  src += user_common_source;
  src += '\n';
  src += user_source;
  src += postfix;
  assert(src.size() == src_size);
  return src;
}

std::string App::assembleShaderSourceAtIndex(int index){
  return concatenateShaderSource(m_template_shader_source_prefixes[index],
                                 m_common_uniforms_shader_source,
                                 m_user_shader_sources[0],
                                 m_user_shader_sources[index + 1],
                                 m_template_shader_source_postfixes[index]);
}

std::string_view App::getUserShaderSourceAtIndex(int index) {
  assert(index >= 0 && index < arraySize(m_user_shader_sources));
  return m_user_shader_sources[index];
}

std::string_view App::getAssembledShaderSourceAtIndex(int index) {
  assert(index >= 0 && index < arraySize(m_assembled_shader_sources));
  return m_assembled_shader_sources[index];
}

void App::setUserShaderSourceAtIndex(int index, std::string_view shader_src) {
  assert(index >= 0 && index < arraySize(m_user_shader_sources));

  m_user_shader_sources[index] = shader_src;

  if (index > 0) {
    m_assembled_shader_sources[index - 1] = assembleShaderSourceAtIndex(index - 1);
  }
}

void App::tryCompileShaderPrograms() {
  gl::Program programs[2];

  gl::createProgram(programs[0], m_simulate_shader_vs_source, m_assembled_shader_sources[0]);
  gl::createProgram(programs[1], m_assembled_shader_sources[1], m_assembled_shader_sources[2]);

  const GLint uniformSamplerLocations[] = { 0, 1, 2, 3, 4, 5 };

  for (size_t i = 0; i < arraySize(programs); ++i) {
    if (programs[i].id) {
      gl::useProgram(programs[i]);
      gl::uniformBlockBinding(programs[i], "CommonUniforms", 0);
      gl::uniform(programs[i], "iFragData[0]", uniformSamplerLocations);

      m_programs[i] = std::move(programs[i]);
    }
  }
}

void App::setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values) {
  std::copy_n(view_matrix_values, 16, &m_common_uniforms.model_view[0][0]);
  std::copy_n(projection_matrix_values, 16, &m_common_uniforms.projection[0][0]);

  updateViewAndProjectionTransforms();
}

void App::setControllerAtIndex(int index,
                               const float *position_values,
                               const float *velocity_values,
                               const float *orientation_values,
                               const float *buttons_values) {
  assert(index >= 0 && index < arraySize(m_controller_position));

  std::copy_n(position_values, 3, &m_controller_position[index][0]);
  std::copy_n(velocity_values, 3, &m_common_uniforms.controller_velocity[index][0]);
  std::copy_n(orientation_values, 4, &m_controller_orientation[index][0]);
  std::copy_n(buttons_values, 4, &m_common_uniforms.controller_buttons[index][0]);
}

double App::getAverageFramesPerSecond() const {
  return m_clock.average_fps;
}
