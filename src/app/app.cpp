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

  m_uniform_resolution = gl::vec2(m_particle_framebuffer_resolution);
  m_uniform_frame = GLint(m_clock.elapsed_frames);
  m_uniform_time = m_clock.elapsed_seconds;
  m_uniform_time_delta = m_clock.elapsed_seconds_delta;

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
    gl::uniform(m_simulate_prog, "iResolution", m_uniform_resolution);
    gl::uniform(m_simulate_prog, "iFrame", m_uniform_frame);
    gl::uniform(m_simulate_prog, "iTime", m_uniform_time);
    gl::uniform(m_simulate_prog, "iTimeDelta", m_uniform_time_delta);

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
    gl::useProgram(m_texture_prog);
    gl::uniform(m_texture_prog, "iModelViewProjection", proj_view_matrix);
    gl::uniform(m_texture_prog, "iResolution", m_uniform_resolution);
    gl::uniform(m_texture_prog, "iFrame", m_uniform_frame);
    gl::uniform(m_texture_prog, "iTime", m_uniform_time);
    gl::uniform(m_texture_prog, "iTimeDelta", m_uniform_time_delta);

    gl::drawVertexBuffer(m_particles_vb);
  }

  CHECK_GL_ERROR();
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

    m_texture_prog = std::move(prog);
  }
}

void App::setViewMatrix(const float *values) {
  std::copy_n(values, 16, &m_view_matrix.value[0][0]);
}

void App::setProjectionMatrix(const float *values) {
  std::copy_n(values, 16, &m_projection_matrix.value[0][0]);
}
