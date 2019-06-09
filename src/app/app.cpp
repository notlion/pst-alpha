#include "app/app.hpp"

#include "app/log.hpp"
#include "app/util.hpp"
#include "app/shaders.hpp"

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
  splitShaderSource(shader_source_simulate, "{simulation}", m_simulation_shader_source_prefix, m_simulation_shader_source_postfix);
  splitShaderSource(shader_source_texture, "{texture}", m_texture_shader_source_prefix, m_texture_shader_source_postfix);

  setSimulationShaderSource(shader_source_user_default_simulation);
  setTextureShaderSource(shader_source_user_default_texture);

  gl::DefaultVertex fs_tri_verts[]{
    { gl::vec3(-1.0f, -1.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) },
    { gl::vec3( 3.0f, -1.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) },
    { gl::vec3(-1.0f,  3.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) }
  };
  gl::DefaultTriangleMesh fs_tri_mesh{
    { { fs_tri_verts[0], fs_tri_verts[1], fs_tri_verts[2] } },
    gl::DefaultVertex::default_attribs
  };
  gl::createVertexBuffer(m_fullscreen_triangle_vb, fs_tri_mesh);
  gl::assignVertexBufferAttributeLocations(m_fullscreen_triangle_vb, { 0, -1, -1 });

  // Create particle vertex buffer
  {
    const auto vertex_count = m_particle_framebuffer_resolution * m_particle_framebuffer_resolution;

    std::vector<gl::ivec2> vertices;
    vertices.reserve(vertex_count);
    for (GLint y = 0; y < m_particle_framebuffer_resolution; ++y) {
      for (GLint x = 0; x < m_particle_framebuffer_resolution; ++x) {
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
      gl::createFramebuffer(*m_particle_fbs[i], m_particle_framebuffer_resolution, m_particle_framebuffer_resolution, {
        { GL_COLOR_ATTACHMENT0, particle_tex_opts },
        { GL_COLOR_ATTACHMENT1, particle_tex_opts },
        { GL_COLOR_ATTACHMENT2, particle_tex_opts },
        { GL_COLOR_ATTACHMENT3, particle_tex_opts },
      });
    }
  }

  m_view_matrix = gl::lookAt(gl::vec3(0.0f, 0.0f, 3.0f), gl::vec3(0.0f), gl::vec3(0.0f, 1.0f, 0.0f));
  m_projection_matrix = gl::perspective(radians(60.0f), 1.0f, 0.01f, 1000.0f);

  return true;
}

void App::cleanup() {
}

void App::setCommonShaderUniforms(gl::Program &prog) {
  gl::uniform(prog, "iModelViewProjection", m_view_projection_matrix);
  gl::uniform(prog, "iModelView", m_view_matrix);
  gl::uniform(prog, "iProjection", m_projection_matrix);
  gl::uniform(prog, "iInverseModelViewProjection", m_inverse_view_projection_matrix);
  gl::uniform(prog, "iInverseModelView", m_inverse_view_matrix);
  gl::uniform(prog, "iInverseProjection", m_inverse_projection_matrix);
  gl::uniform(prog, "iFrame", GLint(m_clock.elapsed_frames));
  gl::uniform(prog, "iTime", GLfloat(m_clock.elapsed_seconds));
  gl::uniform(prog, "iTimeDelta", GLfloat(m_clock.elapsed_seconds_delta));
}

void App::update(double time_seconds) {
  m_clock.tick(time_seconds);

  // Simulate
  {
    std::rotate(m_particle_fbs.rbegin(), m_particle_fbs.rbegin() + 1, m_particle_fbs.rend());

    gl::bindFramebuffer(*m_particle_fbs[0]);

    gl::disableBlend();
    gl::disableDepth();

    glViewport(0, 0, m_particle_framebuffer_resolution, m_particle_framebuffer_resolution);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gl::bindTexture(m_particle_fbs[1]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[2]->textures[0], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[1]->textures[1], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[2]->textures[1], GL_TEXTURE3);

    gl::useProgram(m_simulate_prog);
    gl::uniform(m_simulate_prog, "iResolution", gl::vec2(m_particle_framebuffer_resolution));
    setCommonShaderUniforms(m_simulate_prog);

    gl::drawVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();

    CHECK_GL_ERROR();
  }
}

void App::render(int width, int height) {
  // Texture
  {
    glViewport(0, 0, width, height);

    gl::enableBlendAlphaPremult();
    gl::enableDepth();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl::bindTexture(m_particle_fbs[0]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[0]->textures[1], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[0]->textures[2], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[0]->textures[3], GL_TEXTURE3);

    gl::useProgram(m_texture_prog);
    gl::uniform(m_texture_prog, "iResolution", gl::vec2(width, height));
    setCommonShaderUniforms(m_texture_prog);

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

std::string_view App::getSimulationShaderSource() {
  return m_user_simulation_shader_source;
}

void App::setSimulationShaderSource(std::string_view shader_src) {
  m_user_simulation_shader_source = shader_src;

  auto src = concatenateShaderSource(m_simulation_shader_source_prefix, m_user_simulation_shader_source, m_simulation_shader_source_postfix);
  auto prog = gl::createProgram(src, gl::SHADER_VERSION_300ES);

  if (prog.id) {
    gl::useProgram(prog);
    gl::uniform(prog, "iPosition", 0);
    gl::uniform(prog, "iPositionPrev", 1);
    gl::uniform(prog, "iColor", 2);
    gl::uniform(prog, "iColorPrev", 3);

    m_simulate_prog = std::move(prog);
  }
}

std::string_view App::getTextureShaderSource() {
  return m_user_texture_shader_source;
}

void App::setTextureShaderSource(std::string_view shader_src) {
  m_user_texture_shader_source = shader_src;

  auto src = concatenateShaderSource(m_texture_shader_source_prefix, m_user_texture_shader_source, m_texture_shader_source_postfix);
  auto prog = gl::createProgram(src, gl::SHADER_VERSION_300ES);

  if (prog.id) {
    gl::useProgram(prog);
    gl::uniform(prog, "iPosition", 0);
    gl::uniform(prog, "iColor", 1);
    gl::uniform(prog, "iRight", 2);
    gl::uniform(prog, "iUp", 3);

    m_texture_prog = std::move(prog);
  }
}

void App::updateViewProjectionMatrices() {
  m_view_projection_matrix = m_projection_matrix * m_view_matrix;
  m_inverse_view_projection_matrix = gl::inverse(m_view_projection_matrix);
}

void App::setViewMatrix(const float *values) {
  std::copy_n(values, 16, &m_view_matrix.value[0][0]);
  m_inverse_view_matrix = gl::inverse(m_view_matrix);
  updateViewProjectionMatrices();
}

void App::setProjectionMatrix(const float *values) {
  std::copy_n(values, 16, &m_projection_matrix.value[0][0]);
  m_inverse_projection_matrix = gl::inverse(m_projection_matrix);
  updateViewProjectionMatrices();
}
