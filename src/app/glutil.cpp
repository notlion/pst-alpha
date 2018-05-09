#include "app/glutil.hpp"
#include "app/log.hpp"
#include "app/util.hpp"

#include <cstdarg>
#include <memory>

#if !defined(PLATFORM_EMSCRIPTEN)
  #define STB_IMAGE_IMPLEMENTATION
  #define STBI_NO_STDIO
  #include "stb_image.h"
#endif

namespace gl {

constexpr std::size_t ERROR_LOG_MAX_SIZE = 256;

static std::vector<std::string> s_error_log;

static void logError(const char *err, std::string::size_type len = 0) {
  if (s_error_log.size() == ERROR_LOG_MAX_SIZE) {
    std::copy(s_error_log.begin() + 1, s_error_log.end(), s_error_log.begin());
    s_error_log.pop_back();
  }

  if (len == 0) len = std::strlen(err);

  s_error_log.emplace_back(err, len);

  PRINT_ERROR("%s\n", s_error_log.back().c_str());
}

static void logErrorFmt(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  std::vector<char> buffer(1 + std::vsnprintf(nullptr, 0, fmt, args));
  std::vsnprintf(buffer.data(), buffer.size(), fmt, args);
  va_end(args);

  logError(buffer.data(), buffer.size() - 1);
}

static const char *getErrorString(GLenum err) {
  switch (err) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    default: return "";
  }
}

void checkError() {
  auto err = glGetError();
  if (err != GL_NO_ERROR) {
    logErrorFmt("GL error flag set: %s", getErrorString(err));
  }
}

void clearErrorLog() {
  s_error_log.clear();
}

const std::vector<std::string> &getErrorLog() {
  return s_error_log;
}


GLuint createShader(std::string_view shader_src, GLenum type) {
  auto shader = glCreateShader(type);

  assert(shader_src.size() < std::numeric_limits<int>::max());

  const GLchar *src[]{ shader_src.data() };
  const GLint len[]{ static_cast<GLint>(shader_src.size()) };

  glShaderSource(shader, 1, src, len);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLint log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0) {
      std::unique_ptr<GLchar[]> log(new GLchar[log_length]);
      glGetShaderInfoLog(shader, log_length, nullptr, log.get());
      logError(log.get(), log_length - 1);
    }

    return 0;
  }

  CHECK_GL_ERROR();

  return shader;
}


static void cacheActiveUniforms(Program &prog) {
  GLint active_uniform_count;
  glGetProgramiv(prog.id, GL_ACTIVE_UNIFORMS, &active_uniform_count);

  char name[512];
  GLsizei name_length;
  GLint count;
  GLenum type;

  for (GLint i = 0; i < active_uniform_count; ++i) {
    glGetActiveUniform(prog.id, i, 511, &name_length, &count, &type, name);
    auto loc = glGetUniformLocation(prog.id, name);
    if (loc >= 0) {
      prog.uniforms.push_back({ loc, count, type, { name, std::string::size_type(name_length) } });
    }
  }

  CHECK_GL_ERROR();
}

// static void cacheActiveUniformBlocks(Program &prog) {
//   GLint active_uniform_block_count;
//   glGetProgramiv(prog.id, GL_ACTIVE_UNIFORM_BLOCKS, &active_uniform_block_count);

//   GLint max_name_length;
//   glGetProgramiv(prog.id, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_name_length);

//   char name[max_name_length];
//   GLsizei name_length;

//   for (GLint i = 0; i < active_uniform_block_count; ++i) {
//     glGetActiveUniformBlockName(prog.id, i, max_name_length, &name_length, &name);
//   }

//   CHECK_GL_ERROR();
// }

static void cacheActiveAttribs(Program &prog) {
  GLint active_attrib_count;
  glGetProgramiv(prog.id, GL_ACTIVE_ATTRIBUTES, &active_attrib_count);

  char name[512];
  GLsizei name_length;
  GLint count;
  GLenum type;

  for (GLint i = 0; i < active_attrib_count; ++i) {
    glGetActiveAttrib(prog.id, i, 511, &name_length, &count, &type, name);
    auto loc = glGetAttribLocation(prog.id, name);
    if (loc >= 0) {
      prog.attributes.emplace_back();
      prog.attributes.back().name = { name, std::string::size_type(name_length) };
      prog.attributes.back().loc = loc;
      prog.attributes.back().count = count;
      prog.attributes.back().type = type;
    }
  }

  CHECK_GL_ERROR();
}

Program createProgram(std::string_view vert_shader_src, std::string_view frag_shader_src) {
  Program prog;
  createProgram(prog, vert_shader_src, frag_shader_src);
  return prog;
}

void createProgram(Program &prog, std::string_view vert_shader_src, std::string_view frag_shader_src) {
  deleteProgram(prog);

  auto vshader = createShader(vert_shader_src, GL_VERTEX_SHADER);
  auto fshader = createShader(frag_shader_src, GL_FRAGMENT_SHADER);

  if (vshader && fshader) {
    prog.id = glCreateProgram();

    glAttachShader(prog.id, vshader);
    glAttachShader(prog.id, fshader);

    glLinkProgram(prog.id);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    GLint status;
    glGetProgramiv(prog.id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
      GLint log_length = 0;
      glGetProgramiv(prog.id, GL_INFO_LOG_LENGTH, &log_length);

      if (log_length > 0) {
        std::unique_ptr<GLchar[]> log(new GLchar[log_length]);
        glGetProgramInfoLog(prog.id, log_length, nullptr, log.get());
        logError(log.get(), log_length - 1);
      }

      deleteProgram(prog);

      return;
    }

    cacheActiveUniforms(prog);
    cacheActiveAttribs(prog);

    PRINT_DEBUG("Created program %u\n", prog.id);
  }

  CHECK_GL_ERROR();
}

Program createProgram(std::string_view shader_src, ShaderVersion version) {
  Program prog;
  createProgram(prog, shader_src, version);
  return prog;
}

void createProgram(Program &prog, std::string_view shader_src, ShaderVersion version) {
  using namespace std::string_literals;

  const auto &version_str = [&] {
    switch (version) {
      case SHADER_VERSION_100:   return "#version 100\n"s;
      case SHADER_VERSION_300ES: return "#version 300 es\n"s;
    }
    return ""s;
  }();

  auto vertex_src = version_str + "#define VERTEX_SHADER\n"s;
  vertex_src += shader_src;

  auto fragment_src = version_str + "#define FRAGMENT_SHADER\n"s;
  fragment_src += shader_src;

  createProgram(prog, vertex_src, fragment_src);
}

void deleteProgram(Program &prog) noexcept {
  if (prog.id) {
    PRINT_DEBUG("Deleting program %u\n", prog.id);

    glDeleteProgram(prog.id);

    prog.id = 0;
    prog.uniforms.clear();
    prog.attributes.clear();
  }
}

GLint getUniformLocation(const Program &prog, const GLchar *name) {
  const auto it = std::find_if(prog.uniforms.begin(),
                               prog.uniforms.end(),
                               [&](const Uniform &uniform) { return uniform.name == name; });
  return it == prog.uniforms.end() ? -1 : it->loc;
}

GLint getAttribLocation(const Program &prog, const GLchar *name) {
  const auto it = std::find_if(prog.attributes.begin(),
                               prog.attributes.end(),
                               [&](const Attribute &attrib) { return attrib.name == name; });
  return it == prog.attributes.end() ? -1 : it->loc;
}


Texture createTexture(int width, int height, const TextureOpts &opts) {
  Texture tex;
  createTexture(tex, width, height, opts);
  return tex;
}

Texture createTexture(const TextureData &data, const TextureOpts &opts) {
  Texture tex;
  createTexture(tex, data, opts);
  return tex;
}

void createTexture(Texture &tex, int width, int height, const TextureOpts &opts) {
  deleteTexture(tex);

  tex.width = width;
  tex.height = height;

  glGenTextures(1, &tex.id);
  glBindTexture(opts.target, tex.id);

  glTexImage2D(opts.target, 0, opts.internal_format, width, height, 0, opts.format, opts.component_type, nullptr);

  glTexParameteri(opts.target, GL_TEXTURE_MIN_FILTER, opts.min_filter);
  glTexParameteri(opts.target, GL_TEXTURE_MAG_FILTER, opts.mag_filter);
  glTexParameteri(opts.target, GL_TEXTURE_WRAP_S, opts.wrapS);
  glTexParameteri(opts.target, GL_TEXTURE_WRAP_T, opts.wrapT);

  glBindTexture(opts.target, 0);

  CHECK_GL_ERROR();
}

void createTexture(Texture &tex, const TextureData &data, const TextureOpts &opts) {
  deleteTexture(tex);

  tex.width = data.width;
  tex.height = data.height;
  tex.opts = opts;

  glGenTextures(1, &tex.id);
  glBindTexture(opts.target, tex.id);

  glTexImage2D(opts.target, 0, opts.internal_format, data.width, data.height, 0, opts.format, opts.component_type, data.pixels.get());

  glTexParameteri(opts.target, GL_TEXTURE_MIN_FILTER, opts.min_filter);
  glTexParameteri(opts.target, GL_TEXTURE_MAG_FILTER, opts.mag_filter);
  glTexParameteri(opts.target, GL_TEXTURE_WRAP_S, opts.wrapS);
  glTexParameteri(opts.target, GL_TEXTURE_WRAP_T, opts.wrapT);

  if (opts.min_filter == GL_NEAREST_MIPMAP_NEAREST || opts.min_filter == GL_LINEAR_MIPMAP_NEAREST ||
      opts.min_filter == GL_NEAREST_MIPMAP_LINEAR || opts.min_filter == GL_LINEAR_MIPMAP_LINEAR) {
    glGenerateMipmap(opts.target);
  }

  glBindTexture(opts.target, 0);

  CHECK_GL_ERROR();
}

void deleteTexture(Texture &tex) noexcept {
  if (tex.id > 0) {
    glDeleteTextures(1, &tex.id);
    tex.id = 0;
  }
}


Renderbuffer createRenderbuffer(int width, int height, const RenderbufferOpts &opts) {
  Renderbuffer rb;
  createRenderbuffer(rb, width, height, opts);
  return rb;
}

void createRenderbuffer(Renderbuffer &rb, int width, int height, const RenderbufferOpts &opts) {
  rb.width = width;
  rb.height = height;

  glGenRenderbuffers(1, &rb.id);
  glBindRenderbuffer(opts.target, rb.id);
  glRenderbufferStorage(opts.target, opts.format, width, height);
  glBindRenderbuffer(opts.target, 0);

  CHECK_GL_ERROR();
}

void deleteRenderbuffer(Renderbuffer &rb) noexcept {
  if (rb.id > 0) glDeleteRenderbuffers(1, &rb.id);
  rb.id = 0;
}


static const char *getFramebufferStatusString(GLenum status) {
  switch (status) {
    case GL_FRAMEBUFFER_UNSUPPORTED: return "Unsupported framebuffer format";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "Framebuffer incomplete: missing attachment";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "Framebuffer incomplete: incomplete attachment";
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: return "Framebuffer incomplete: not all attached images have the same number of samples";
    default: return "Framebuffer invalid: unknown reason";
  }
}

Framebuffer createFramebuffer(int width, int height,
                              const std::vector<FramebufferTextureAttachment> &texture_attachments,
                              const std::vector<FramebufferRenderbufferAttachment> &renderbuffer_attachments) {
  Framebuffer fb;
  createFramebuffer(fb, width, height, texture_attachments, renderbuffer_attachments);
  return fb;
}

void createFramebuffer(Framebuffer &fb, int width, int height,
                       const std::vector<FramebufferTextureAttachment> &texture_attachments,
                       const std::vector<FramebufferRenderbufferAttachment> &renderbuffer_attachments) {
  deleteFramebuffer(fb);

  fb.width = width;
  fb.height = height;

  glGenFramebuffers(1, &fb.id);
  glBindFramebuffer(GL_FRAMEBUFFER, fb.id);

  fb.textures.reserve(texture_attachments.size());
  fb.buffers.reserve(texture_attachments.size());

  for (const auto &ta : texture_attachments) {
    fb.buffers.emplace_back(ta.attachment);
    fb.textures.emplace_back();
    createTexture(fb.textures.back(), width, height, ta.opts);
    glFramebufferTexture2D(GL_FRAMEBUFFER, ta.attachment, ta.opts.target, fb.textures.back().id, 0);
  }

  fb.renderbuffers.reserve(renderbuffer_attachments.size());

  for (const auto &ra : renderbuffer_attachments) {
    fb.renderbuffers.emplace_back();
    createRenderbuffer(fb.renderbuffers.back(), width, height, ra.opts);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, ra.attachment, ra.opts.target, fb.renderbuffers.back().id);
  }

  auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    logError(getFramebufferStatusString(status));
    assert(0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  CHECK_GL_ERROR();
}

void deleteFramebuffer(Framebuffer &fb) noexcept {
  for (auto &tex : fb.textures) deleteTexture(tex);
  for (auto &rb : fb.renderbuffers) deleteRenderbuffer(rb);

  if (fb.id > 0) {
    glDeleteFramebuffers(1, &fb.id);
    fb.id = 0;
  }
}


void bindFramebuffer(const Framebuffer &fb) {
  glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
  glDrawBuffers(fb.buffers.size(), fb.buffers.data());
}


Program::Program(Program &&prog) noexcept
: uniforms(std::move(prog.uniforms)), attributes(std::move(prog.attributes)) {
  deleteProgram(*this);
  id = prog.id;
  prog.id = 0;
}

Program &Program::operator=(Program &&prog) noexcept {
  if (this != &prog) {
    deleteProgram(*this);
    id = prog.id;

    uniforms = std::move(prog.uniforms);
    attributes = std::move(prog.attributes);

    prog.id = 0;
  }
  return *this;
}

Program::~Program() noexcept {
  deleteProgram(*this);
}


Texture::Texture(Texture &&tex) noexcept
: width(std::move(tex.width)), height(std::move(tex.height)), opts(std::move(tex.opts)) {
  deleteTexture(*this);
  id = tex.id;
  tex.id = 0;
}

Texture &Texture::operator=(Texture &&tex) noexcept {
  if (this != &tex) {
    deleteTexture(*this);
    id = tex.id;

    width = std::move(tex.width);
    height = std::move(tex.height);
    opts = std::move(tex.opts);

    tex.id = 0;
  }
  return *this;
}

Texture::~Texture() noexcept {
  deleteTexture(*this);
}


Renderbuffer::Renderbuffer(Renderbuffer &&rb) noexcept
: width(std::move(rb.width)), height(std::move(rb.height)) {
  deleteRenderbuffer(*this);
  id = rb.id;
  rb.id = 0;
}

Renderbuffer &Renderbuffer::operator=(Renderbuffer &&rb) noexcept {
  if (this != &rb) {
    deleteRenderbuffer(*this);
    id = rb.id;

    width = std::move(rb.width);
    height = std::move(rb.height);

    rb.id = 0;
  }
  return *this;
}

Renderbuffer::~Renderbuffer() noexcept {
  deleteRenderbuffer(*this);
}


Framebuffer::Framebuffer(Framebuffer &&fb) noexcept
: width(std::move(fb.width)),
  height(std::move(fb.height)),
  buffers(std::move(fb.buffers)) {
  deleteFramebuffer(*this);
  id = fb.id;

  textures = std::move(fb.textures);
  renderbuffers = std::move(fb.renderbuffers);

  fb.id = 0;
}

Framebuffer &Framebuffer::operator=(Framebuffer &&fb) noexcept {
  if (this != &fb) {
    deleteFramebuffer(*this);
    id = fb.id;

    width = std::move(fb.width);
    height = std::move(fb.height);
    textures = std::move(fb.textures);
    renderbuffers = std::move(fb.renderbuffers);
    buffers = std::move(fb.buffers);

    fb.id = 0;
  }
  return *this;
}

Framebuffer::~Framebuffer() noexcept {
  deleteFramebuffer(*this);
}


} // gl
