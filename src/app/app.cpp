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

  // Alloc particle data framebuffers
  {
    for (size_t i = 0; i < arraySize(m_particle_fbs); ++i) {
      m_particle_fbs[i] = std::make_unique<gl::Framebuffer>();
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

    m_common_uniforms.size = m_particle_framebuffer_resolution;

    m_common_uniforms.time = float(time_seconds);
    m_common_uniforms.time_delta = float(time_delta_seconds);
    m_common_uniforms.frame = frame_id;
  }
}

void App::render(int width, int height) {
  gl::updateUniformBuffer(m_common_uniforms_buffer, m_common_uniforms);

  // Create particle data framebuffers (if needed)
  {
    for (size_t i = 0; i < arraySize(m_particle_fbs); ++i) {
      if (m_particle_fbs[i]->width != m_particle_framebuffer_resolution.x || m_particle_fbs[i]->height != m_particle_framebuffer_resolution.y) {
        gl::TextureOpts particle_tex_opts{ GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST };
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
    gl::uniform(m_programs[0], "iResolution", gl::ivec2(width, height));

    gl::drawVertexBuffer(m_fullscreen_triangle_vb);

    gl::unbindFramebuffer();

    CHECK_GL_ERROR();
  }

  // Texture
  {
    gl::disableBlend();
    gl::enableDepth();

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl::bindTexture(m_particle_fbs[0]->textures[0], GL_TEXTURE0);
    gl::bindTexture(m_particle_fbs[0]->textures[1], GL_TEXTURE1);
    gl::bindTexture(m_particle_fbs[0]->textures[2], GL_TEXTURE2);
    gl::bindTexture(m_particle_fbs[0]->textures[3], GL_TEXTURE3);
    gl::bindTexture(m_particle_fbs[0]->textures[4], GL_TEXTURE4);
    gl::bindTexture(m_particle_fbs[0]->textures[5], GL_TEXTURE5);

    gl::bindUniformBuffer(m_common_uniforms_buffer, 0);

    gl::useProgram(m_programs[1]);
    gl::uniform(m_programs[1], "iResolution", gl::ivec2(width, height));

    GLsizei instance_count = m_particle_framebuffer_resolution.x * m_particle_framebuffer_resolution.y;
    glDrawArrays(GL_TRIANGLES, 0, m_instance_vertex_count * instance_count);

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


struct ShaderPragma {
  std::vector<std::string> args;
};

static const std::string SHADER_PRAGMA_PREFIX{ "#pragma" }; 

static std::string_view findNextToken(std::string_view::iterator &begin, std::string_view::iterator end) {
  begin = std::find_if(begin, end, [](auto c) { return !std::isspace(c); });
  
  std::string_view token;
  
  if (begin != end) {
    auto tokenEnd = std::find_if(begin, end, [](auto c) { return std::isspace(c); });
    token = std::string_view(begin, tokenEnd - begin);
    begin = tokenEnd;
  }

  return token;
}

static std::vector<ShaderPragma> parsePragmas(std::string_view source) {
  std::vector<ShaderPragma> pragmas;
  
  for (auto lineBegin = source.begin(); lineBegin != source.end();) {
    auto lineEnd = std::find(lineBegin, source.end(), '\n');
    if (lineEnd != source.end()) lineEnd++;

    // Skip leading whitespace
    lineBegin = std::find_if(lineBegin, lineEnd, [](auto c) { return !std::isspace(c); });
    if (lineBegin == lineEnd) continue;

    // Check if the line starts with "#pragma"
    if (lineEnd - lineBegin >= SHADER_PRAGMA_PREFIX.size() && std::equal(SHADER_PRAGMA_PREFIX.begin(), SHADER_PRAGMA_PREFIX.end(), lineBegin)) {
      pragmas.emplace_back();
      
      lineBegin += SHADER_PRAGMA_PREFIX.size();

      // Find and append arguments
      while (lineBegin != lineEnd) {
        const auto arg = findNextToken(lineBegin, lineEnd);
        if (!arg.empty()) {
          pragmas.back().args.emplace_back(arg);
        }
      }
    }
    else {
      lineBegin = lineEnd;
    }
  }

  return pragmas;
}

void App::parseSimulationShaderPragmas() {
  const auto pragmas = parsePragmas(m_user_shader_sources[1]);
  for (const auto &pragma : pragmas) {
    if (pragma.args.size() == 3 && pragma.args[0] == "size") {
      gl::ivec2 size{ std::atoi(pragma.args[1].c_str()), std::atoi(pragma.args[2].c_str()) };
      if (size.x > 0 && size.y > 0) {
        m_particle_framebuffer_resolution = size;
      }
      else {
        m_particle_framebuffer_resolution = m_default_particle_framebuffer_resolution;
      }
    }
  }
}

void App::parseRenderShaderPragmas() {
  const auto vertexPragmas = parsePragmas(m_user_shader_sources[2]);
  for (const auto &pragma : vertexPragmas) {
    if (pragma.args.size() == 2 && pragma.args[0] == "vertexCount") {
      int count = std::atoi(pragma.args[1].c_str());
      if (count > 0) {
        m_instance_vertex_count = count;
      }
      else {
        m_instance_vertex_count = m_default_instance_vertex_count;
      }
    }
  }
}

bool App::tryCompileShaderPrograms() {
  gl::Program programs[2];

  gl::ProgramError programError;

  if (!gl::createProgram(programs[0], m_simulate_shader_vs_source, m_assembled_shader_sources[0], &programError)) {
    return false;
  }
  parseSimulationShaderPragmas();

  if (!gl::createProgram(programs[1], m_assembled_shader_sources[1], m_assembled_shader_sources[2], &programError)) {
    return false;
  }
  parseRenderShaderPragmas();

  const GLint uniformSamplerLocations[] = { 0, 1, 2, 3, 4, 5 };

  for (size_t i = 0; i < arraySize(programs); ++i) {
    if (programs[i].id) {
      gl::useProgram(programs[i]);
      gl::uniformBlockBinding(programs[i], "CommonUniforms", 0);
      gl::uniform(programs[i], "iFragData[0]", uniformSamplerLocations);

      m_programs[i] = std::move(programs[i]);
    }
  }

  return true;
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
