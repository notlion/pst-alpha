#include "app/app.hpp"

#include "app/log.hpp"
#include "app/util.hpp"
#include "app/shaders.hpp"

bool App::init() {
  randSeed(uint32_t(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));

  m_clock.start();

  gl::createProgram(m_mesh_prog, shader_source_mesh, gl::SHADER_VERSION_300ES);

  gl::createTriangleMeshVertexBuffer(m_mesh_vb, gl::createQuad());
  gl::assignTriangleMeshVertexBufferAttributeLocations(m_mesh_vb, { 0, 1, 2 });

  gl::DefaultTriangleMesh fs_tri_mesh{
    {
      { gl::vec3(-1.0f, -1.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) },
      { gl::vec3( 2.0f, -1.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) },
      { gl::vec3(-1.0f,  2.0f, 0.0f), gl::vec3(0.0f, 0.0f, 1.0f), gl::vec2(0.0f, 0.0f) }
    },
    gl::DefaultVertex::default_attribs
  };
  gl::createTriangleMeshVertexBuffer(m_fullscreen_triangle_vb, fs_tri_mesh);

  gl::createProgram(m_simulate_prog, shader_source_simulate, gl::SHADER_VERSION_300ES);
  gl::createProgram(m_render_prog, shader_source_render, gl::SHADER_VERSION_300ES);

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

    glViewport(0, 0, m_particle_framebuffer_resolution, m_particle_framebuffer_resolution);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gl::useProgram(m_simulate_prog);

    // gl::drawTriangleMeshVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();
  }
}

void App::render(int width, int height) {
#if 1
  {
    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float render_aspect = float(width) / float(height);

    const auto view_matrix = gl::lookAt(m_camera.position, m_camera.target, m_camera.up);
    const auto proj_matrix = gl::perspective(radians(m_camera.fovy), render_aspect, 0.01f, 1000.0f);
    const auto proj_view_matrix = proj_matrix * view_matrix;
    const auto light_matrix = gl::transpose(gl::inverse(gl::mat3(proj_view_matrix)));

    gl::useProgram(m_mesh_prog);
    
    const auto model_matrix = gl::mat4();
    const auto mvp_matrix = proj_view_matrix * model_matrix;
    const auto normal_matrix = gl::transpose(gl::inverse(gl::mat3(mvp_matrix)));

    gl::uniform(m_mesh_prog, "u_mvp_matrix", mvp_matrix);
    gl::uniform(m_mesh_prog, "u_normal_matrix", normal_matrix);

    gl::drawTriangleMeshVertexBuffer(m_mesh_vb);
  }
#else
  {
  }
#endif

  CHECK_GL_ERROR();
}
