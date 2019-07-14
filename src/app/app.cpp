#include "app/app.hpp"

#include "app/log.hpp"
#include "app/shaders.hpp"
#include "app/util.hpp"

using namespace std::string_literals;

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
  PRINT_DEBUG("CommonUniforms Size: %lu\n", sizeof(CommonShaderUniforms));

  splitShaderSource(shader_source_simulate, "{simulation}", m_user_shader_source_prefixes[0], m_user_shader_source_postfixes[0]);
  splitShaderSource(shader_source_texture, "{texture}", m_user_shader_source_prefixes[1], m_user_shader_source_postfixes[1]);

  setUserShaderSourceAtIndex(0, shader_source_user_default_simulation);
  setUserShaderSourceAtIndex(1, shader_source_user_default_texture);

  gl::DefaultVertex fs_tri_verts[]{
    { gl::vec3(-1.0f, -1.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) },
    { gl::vec3(3.0f, -1.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) },
    { gl::vec3(-1.0f, 3.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) }
  };
  gl::DefaultTriangleMesh fs_tri_mesh{
    { { fs_tri_verts[0], fs_tri_verts[1], fs_tri_verts[2] } },
    gl::DefaultVertex::default_attribs
  };
  gl::createVertexBuffer(m_fullscreen_triangle_vb, fs_tri_mesh);
  gl::assignVertexBufferAttributeLocations(m_fullscreen_triangle_vb, { 0, -1, -1 });

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
      { GL_INT, 2, 0, 0, 0 },
    };

    gl::createVertexBuffer(m_particles_vb, GL_POINTS, sizeof(gl::ivec2) * vertices.size(), vertices.size(), vertices.data(), GL_STATIC_DRAW, attribs);
  }

  // Create particle quad vertex buffer
  {
    auto particle_quad_mesh = gl::createQuad();
    gl::createVertexBuffer(m_particle_quad_vb, particle_quad_mesh, GL_STATIC_DRAW);
    gl::assignVertexBufferAttributeLocations(m_particle_quad_vb, { 1, 2, 3 });
  }

  // Create particle data framebuffers
  {
    gl::TextureOpts particle_tex_opts{ GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST };

    for (size_t i = 0; i < m_particle_fbs.size(); ++i) {
      m_particle_fbs[i] = std::make_unique<gl::Framebuffer>();
      gl::createFramebuffer(*m_particle_fbs[i],
                            m_particle_framebuffer_resolution.x,
                            m_particle_framebuffer_resolution.y,
                            {
                                { GL_COLOR_ATTACHMENT0, particle_tex_opts },
                                { GL_COLOR_ATTACHMENT1, particle_tex_opts },
                                { GL_COLOR_ATTACHMENT2, particle_tex_opts },
                                { GL_COLOR_ATTACHMENT3, particle_tex_opts },
                            });
    }
  }

  {
    m_common_uniforms.model_view = gl::lookAt(gl::vec3(0.0f, 0.0f, 3.0f), gl::vec3(0.0f), gl::vec3(0.0f, 1.0f, 0.0f));
    m_common_uniforms.projection = gl::perspective(radians(60.0f), 1.0f, 0.01f, 1000.0f);

    updateCommonShaderUniformMatrices();

    m_common_uniforms.controller_position[0] = gl::vec4(-0.5f, 1.0f, 0.0f, 1.0f);
    m_common_uniforms.controller_position[1] = gl::vec4(0.5f, 1.0f, 0.0f, 1.0f);

    gl::createUniformBuffer(m_common_uniforms_buffer, m_common_uniforms, GL_DYNAMIC_DRAW);
  }

  return true;
}

void App::cleanup() {
}

void App::updateCommonShaderUniformMatrices() {
  m_common_uniforms.inverse_model_view = gl::inverse(m_common_uniforms.model_view);
  m_common_uniforms.inverse_projection = gl::inverse(m_common_uniforms.projection);
  m_common_uniforms.model_view_projection = m_common_uniforms.projection * m_common_uniforms.model_view;
  m_common_uniforms.inverse_model_view_projection = gl::inverse(m_common_uniforms.model_view_projection);
}

void App::update(double time_seconds) {
  m_clock.tick(time_seconds);

  // Update common uniforms
  {
    m_common_uniforms.time = float(m_clock.elapsed_seconds);
    m_common_uniforms.time_delta = float(m_clock.elapsed_seconds_delta);
    m_common_uniforms.frame = float(m_clock.elapsed_frames);

    gl::updateUniformBuffer(m_common_uniforms_buffer, m_common_uniforms);
  }

  // Simulate
  {
    std::rotate(m_particle_fbs.rbegin(), m_particle_fbs.rbegin() + 1, m_particle_fbs.rend());

    gl::bindFramebuffer(*m_particle_fbs[0]);

    gl::disableBlend();
    gl::disableDepth();

    glViewport(0, 0, m_particle_framebuffer_resolution.x, m_particle_framebuffer_resolution.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gl::bindTexture(m_particle_fbs[1]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[2]->textures[0], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[1]->textures[1], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[2]->textures[1], GL_TEXTURE3);

    gl::bindUniformBuffer(m_common_uniforms_buffer, 0);

    gl::useProgram(m_simulate_prog);
    gl::uniform(m_simulate_prog, "iResolution", gl::vec2(m_particle_framebuffer_resolution));

    gl::drawVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();

    CHECK_GL_ERROR();
  }
}

void App::render(int width, int height) {
  gl::updateUniformBuffer(m_common_uniforms_buffer, m_common_uniforms);

  // Texture
  {
    gl::enableBlendAlphaPremult();
    gl::enableDepth();

    gl::bindTexture(m_particle_fbs[0]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[0]->textures[1], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[0]->textures[2], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[0]->textures[3], GL_TEXTURE3);

    gl::bindUniformBuffer(m_common_uniforms_buffer, 0);

    gl::useProgram(m_texture_prog);
    gl::uniform(m_texture_prog, "iResolution", gl::vec2(width, height));

    gl::enableVertexBuffer(m_particles_vb);
    gl::enableVertexBuffer(m_particle_quad_vb);

    glVertexAttribDivisor(m_particles_vb.attribs[0].loc, 1);
    glVertexAttribDivisor(m_particle_quad_vb.attribs[0].loc, 0);
    glVertexAttribDivisor(m_particle_quad_vb.attribs[1].loc, 0);
    glVertexAttribDivisor(m_particle_quad_vb.attribs[2].loc, 0);

    glDrawArraysInstanced(GL_TRIANGLES, 0, m_particle_quad_vb.count, m_particles_vb.count);

    glVertexAttribDivisor(m_particles_vb.attribs[0].loc, 0);

    gl::disableVertexBuffer(m_particles_vb);
    gl::disableVertexBuffer(m_particle_quad_vb);

    CHECK_GL_ERROR();
  }
}

static std::string concatenateShaderSource(std::string_view prefix, std::string_view user_source, std::string_view postfix) {
  auto src = std::string();
  auto src_size = prefix.size() + user_source.size() + postfix.size() + 1;
  src.reserve(src_size + 1);
  src += prefix;
  src += '\n';
  src += user_source;
  src += postfix;
  assert(src.size() == src_size);
  return src;
}

std::string_view App::getUserShaderSourceAtIndex(int index) {
  assert(index >= 0 && index < arraySize(m_user_shader_sources));
  return m_user_shader_sources[index];
}

void App::setUserShaderSourceAtIndex(int index, std::string_view shader_src) {
  assert(index >= 0 && index < arraySize(m_user_shader_sources));

  m_user_shader_sources[index] = shader_src;

  auto src = concatenateShaderSource(m_user_shader_source_prefixes[index], m_user_shader_sources[index], m_user_shader_source_postfixes[index]);
  auto prog = gl::createProgram(src, gl::SHADER_VERSION_300ES);

  if (prog.id) {
    gl::useProgram(prog);

    switch (index) {
      // Simulate
      case 0: {
        gl::uniform(prog, "iPosition", 0);
        gl::uniform(prog, "iPositionPrev", 1);
        gl::uniform(prog, "iColor", 2);
        gl::uniform(prog, "iColorPrev", 3);
        gl::uniformBlockBinding(prog, "CommonUniforms", 0);

        m_simulate_prog = std::move(prog);
      } break;

      // Shade
      case 1: {
        gl::uniform(prog, "iPosition", 0);
        gl::uniform(prog, "iColor", 1);
        gl::uniform(prog, "iRight", 2);
        gl::uniform(prog, "iUp", 3);
        gl::uniformBlockBinding(prog, "CommonUniforms", 0);

        m_texture_prog = std::move(prog);
      } break;
    }
  }
}

void App::setViewAndProjectionMatrices(const float *view_matrix_values, const float *projection_matrix_values) {
  std::copy_n(view_matrix_values, 16, &m_common_uniforms.model_view.value[0][0]);
  std::copy_n(projection_matrix_values, 16, &m_common_uniforms.projection.value[0][0]);

  updateCommonShaderUniformMatrices();
}

void App::setControllerPoseAtIndex(int index, const float *position_values, const float *velocity_values) {
  assert(index >= 0 && index < arraySize(m_common_uniforms.controller_position));
  std::copy_n(position_values, 3, &m_common_uniforms.controller_position[index][0]);
  std::copy_n(velocity_values, 3, &m_common_uniforms.controller_velocity[index][0]);
}

double App::getAverageFramesPerSecond() const {
  return m_clock.average_fps;
}
