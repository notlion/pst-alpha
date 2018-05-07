#include "app/app.hpp"

#include "app/log.hpp"
#include "app/util.hpp"
#include "app/shaders.hpp"

bool App::init() {
  randSeed(uint32_t(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));

  m_clock.start();

  gl::createProgram(m_mesh_prog, shader_source_mesh, gl::SHADER_VERSION_300ES);
  gl::useProgram(m_mesh_prog);
  gl::uniform(m_mesh_prog, "u_texture", 0);

  gl::createProgram(m_simulate_prog, shader_source_simulate, gl::SHADER_VERSION_300ES);
  gl::useProgram(m_simulate_prog);
  gl::uniform(m_simulate_prog, "u_position", 0);
  gl::uniform(m_simulate_prog, "u_position_prev", 1);
  gl::uniform(m_simulate_prog, "u_color", 2);
  gl::uniform(m_simulate_prog, "u_color_prev", 3);

  gl::createProgram(m_render_prog, shader_source_render, gl::SHADER_VERSION_300ES);
  gl::useProgram(m_render_prog);
  gl::uniform(m_render_prog, "u_position", 0);
  gl::uniform(m_render_prog, "u_color", 1);

  gl::createVertexBuffer(m_mesh_vb, gl::createQuad());
  gl::assignVertexBufferAttributeLocations(m_mesh_vb, { 0, 1, 2 });

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

  m_camera.target = gl::vec3(0.0f);

  return true;
}

void App::cleanup() {
}

void App::update(int timestamp) {
  m_clock.tick();
  m_camera.position = gl::vec3(std::cos(m_clock.elapsed_seconds), std::sin(m_clock.elapsed_seconds), 3.0f);

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
    gl::uniform(m_simulate_prog, "u_resolution", gl::vec2(m_particle_framebuffer_resolution));
    gl::uniform(m_simulate_prog, "u_frame", GLint(m_clock.elapsed_frames));
    gl::uniform(m_simulate_prog, "u_time", m_clock.elapsed_seconds);
    gl::uniform(m_simulate_prog, "u_time_delta", m_clock.elapsed_seconds_delta);

    gl::drawVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();
  }

  CHECK_GL_ERROR();
}

void App::render(int width, int height) {
#if 1
  {
    glViewport(0, 0, width, height);

    gl::enableBlendAlphaPremult();
    gl::enableDepth();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float render_aspect = float(width) / float(height);

    const auto view_matrix = gl::lookAt(m_camera.position, m_camera.target, m_camera.up);
    const auto proj_matrix = gl::perspective(radians(m_camera.fovy), render_aspect, 0.01f, 1000.0f);
    const auto proj_view_matrix = proj_matrix * view_matrix;
    const auto light_matrix = gl::transpose(gl::inverse(gl::mat3(proj_view_matrix)));

    gl::bindTexture(m_particle_fbs[0]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[0]->textures[1], GL_TEXTURE1);

    const auto model_matrix = gl::mat4();
    const auto mvp_matrix = proj_view_matrix * model_matrix;
    const auto normal_matrix = gl::transpose(gl::inverse(gl::mat3(mvp_matrix)));

#if 0
    {
      gl::useProgram(m_mesh_prog);

      gl::uniform(m_mesh_prog, "u_mvp_matrix", mvp_matrix);
      gl::uniform(m_mesh_prog, "u_normal_matrix", normal_matrix);

      gl::drawVertexBuffer(m_mesh_vb);
    }
#endif

    {
      gl::useProgram(m_render_prog);
      gl::uniform(m_render_prog, "u_mvp_matrix", mvp_matrix);

      gl::drawVertexBuffer(m_particles_vb);
    }
  }
#endif

  CHECK_GL_ERROR();
}
