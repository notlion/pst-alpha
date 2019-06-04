#include "app/app.hpp"

#include "app/log.hpp"
#include "app/util.hpp"
#include "app/shaders.hpp"

using namespace std::string_literals;

bool App::init() {
  {
    const auto sim_marker = "{simulation}"s;
    const auto sim_src = std::string_view(shader_source_simulate);
    const auto pos = sim_src.find(sim_marker) + sim_marker.length();
    m_shader_source_simulate_prefix = sim_src.substr(0, pos);
    m_shader_source_simulate_postfix = sim_src.substr(pos, sim_src.length() - pos);
  }

  setShaderSource(shader_source_user_default);

  gl::createProgram(m_render_prog, shader_source_render, gl::SHADER_VERSION_300ES);
  gl::useProgram(m_render_prog);
  gl::uniform(m_render_prog, "iPosition", 0);
  gl::uniform(m_render_prog, "iColor", 1);

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

  {
    std::vector<GLint> texcoords;
    texcoords.reserve(m_particle_framebuffer_resolution);
    for (GLint y = 0; y < m_particle_framebuffer_resolution; ++y) {
      for (GLint x = 0; x < m_particle_framebuffer_resolution; ++x) {
        texcoords.emplace_back(x);
        texcoords.emplace_back(y);
      }
    }

    gl::createVertexBuffer(m_particles_vb, GL_POINTS, sizeof(GLint) * texcoords.size(), texcoords.size() / 2, texcoords.data(), {
      { GL_INT, 2, 0, 0, 0 }
    });
  }

  gl::TextureOpts particle_tex_opts{ GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST };
  for (size_t i = 0; i < m_particle_fbs.size(); ++i) {
    m_particle_fbs[i] = std::make_unique<gl::Framebuffer>();
    gl::createFramebuffer(*m_particle_fbs[i], m_particle_framebuffer_resolution, m_particle_framebuffer_resolution, {
      { GL_COLOR_ATTACHMENT0, particle_tex_opts },
      { GL_COLOR_ATTACHMENT1, particle_tex_opts }
    });
  }

  m_view_matrix = gl::lookAt(gl::vec3(0.0f, 0.0f, 3.0f), gl::vec3(0.0f), gl::vec3(0.0f, 1.0f, 0.0f));
  m_projection_matrix = gl::perspective(radians(60.0f), 1.0f, 0.01f, 1000.0f);

  return true;
}

void App::cleanup() {
}

void App::update(double time_seconds) {
  m_clock.tick(time_seconds);

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
    gl::uniform(m_simulate_prog, "iFrame", GLint(m_clock.elapsed_frames));
    gl::uniform(m_simulate_prog, "iTime", m_clock.elapsed_seconds);
    gl::uniform(m_simulate_prog, "iTimeDelta", m_clock.elapsed_seconds_delta);

    gl::drawVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();
  }

  CHECK_GL_ERROR();
}

void App::render(int width, int height) {
  glViewport(0, 0, width, height);

  gl::enableBlendAlphaPremult();
  gl::enableDepth();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const auto proj_view_matrix = m_projection_matrix * m_view_matrix;

  gl::bindTexture(m_particle_fbs[0]->textures[0], GL_TEXTURE0);
  gl::bindTexture(m_particle_fbs[0]->textures[1], GL_TEXTURE1);

  // const auto normal_matrix = gl::transpose(gl::inverse(gl::mat3(proj_view_matrix)));

  {
    gl::useProgram(m_render_prog);
    gl::uniform(m_render_prog, "iModelViewProjection", proj_view_matrix);

    gl::drawVertexBuffer(m_particles_vb);
  }

  CHECK_GL_ERROR();
}

std::string_view App::getShaderSource() {
  return m_user_shader_source;
}

void App::setShaderSource(std::string_view shader_src) {
  m_user_shader_source = shader_src;

  auto src = std::string();
  auto src_size = m_shader_source_simulate_prefix.size() + m_user_shader_source.size() + m_shader_source_simulate_postfix.size() + 1;
  src.reserve(src_size + 1);
  src += m_shader_source_simulate_prefix;
  src += '\n';
  src += m_user_shader_source;
  src += m_shader_source_simulate_postfix;
  assert(src.size() == src_size);

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

void App::setViewMatrix(const float *values) {
  std::copy_n(values, 16, &m_view_matrix.value[0][0]);
}

void App::setProjectionMatrix(const float *values) {
  std::copy_n(values, 16, &m_projection_matrix.value[0][0]);
}
