#pragma once

#include "app/platform.hpp"

#if defined(PLATFORM_WINDOWS)
  #define NOMINMAX
  #include "glad/glad.h"
  #undef NOMINMAX
#elif defined(PLATFORM_OSX)
  #include "glad/glad.h"
#elif defined(PLATFORM_EMSCRIPTEN)
  #include <GLES3/gl3.h>
  #include <GLES2/gl2ext.h>
#else
  #error "Unsupported Platform"
#endif

#define LC_MINIMATH_NAMESPACE gl
#include "lc_mini_math.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#if defined(NDEBUG)
  #define CHECK_GL_ERROR() ((void)0)
#else
  #define CHECK_GL_ERROR() gl::checkError()
#endif

namespace gl {

#define GL_UTIL_MOVE_ONLY_CLASS(ClassName)          \
  ClassName(const ClassName &) = delete;            \
  ClassName &operator=(const ClassName &) = delete; \
  ClassName(ClassName &&) noexcept;                 \
  ClassName &operator=(ClassName &&) noexcept;      \
  ClassName() noexcept = default;                   \
  ~ClassName() noexcept;

struct Uniform {
  GLint loc = -1;
  GLint count;
  GLenum type;

  std::string name;
};

struct Attribute {
  GLint loc = -1;
  GLenum type;
  GLint count;

  std::string name;
};

enum ShaderVersion {
  SHADER_VERSION_100   = 100,
  SHADER_VERSION_300ES = 300
};

struct Program {
  GLuint id = 0;

  std::vector<Uniform> uniforms;
  std::vector<Attribute> attributes;

  GL_UTIL_MOVE_ONLY_CLASS(Program)
};

struct TextureData {
  int width = 0;
  int height = 0;
  int component_count = 0;

  std::unique_ptr<uint8_t[]> pixels;
};

struct TextureOpts {
  GLenum target = GL_TEXTURE_2D;
  GLenum internal_format = GL_RGBA;
  GLenum format = GL_RGBA;
  GLenum component_type = GL_UNSIGNED_BYTE;

  GLenum min_filter = GL_LINEAR;
  GLenum mag_filter = GL_LINEAR;

  GLenum wrapS = GL_CLAMP_TO_EDGE;
  GLenum wrapT = GL_CLAMP_TO_EDGE;
};

struct Texture {
  GLuint id = 0;

  int width = 0;
  int height = 0;

  TextureOpts opts;

  GL_UTIL_MOVE_ONLY_CLASS(Texture)
};

struct RenderbufferOpts {
  GLenum target = GL_RENDERBUFFER;
  GLenum format = GL_DEPTH_COMPONENT24;  
};

struct Renderbuffer {
  GLuint id = 0;

  int width = 0;
  int height = 0;

  GL_UTIL_MOVE_ONLY_CLASS(Renderbuffer)
};

struct FramebufferTextureAttachment {
  GLenum attachment = GL_COLOR_ATTACHMENT0;
  TextureOpts opts = {};
};

struct FramebufferRenderbufferAttachment {
  GLenum attachment = GL_DEPTH_ATTACHMENT;
  RenderbufferOpts opts = {};
};

struct Framebuffer {
  GLuint id = 0;

  int width = 0;
  int height = 0;

  std::vector<Texture> textures;
  std::vector<Renderbuffer> renderbuffers;

  std::vector<GLenum> buffers;

  GL_UTIL_MOVE_ONLY_CLASS(Framebuffer)
};

void checkError();
void clearErrorLog();
const std::vector<std::string> &getErrorLog();

GLuint createShader(std::string_view shader_src, GLenum type);

Program createProgram(std::string_view vert_shader_src, std::string_view frag_shader_src);
void createProgram(Program &prog, std::string_view vert_shader_src, std::string_view frag_shader_src);
Program createProgram(std::string_view shader_src, ShaderVersion version = SHADER_VERSION_100);
void createProgram(Program &prog, std::string_view shader_src, ShaderVersion version = SHADER_VERSION_100);
void deleteProgram(Program &prog) noexcept;

GLint getUniformLocation(const Program &prog, const GLchar *name);
GLint getAttribLocation(const Program &prog, const GLchar *name);

Texture createTexture(int width, int height, const TextureOpts &opts = {});
Texture createTexture(const TextureData &data, const TextureOpts &opts = {});
void createTexture(Texture &tex, int width, int height, const TextureOpts &opts = {});
void createTexture(Texture &tex, const TextureData &data, const TextureOpts &opts = {});
void deleteTexture(Texture &tex) noexcept;

Renderbuffer createRenderbuffer(int width, int height, const RenderbufferOpts &opts = {});
void createRenderbuffer(Renderbuffer &rb, int width, int height, const RenderbufferOpts &opts = {});
void deleteRenderbuffer(Renderbuffer &rb) noexcept;

Framebuffer createFramebuffer(int width, int height, const std::vector<FramebufferTextureAttachment> &texture_attachments,
                              const std::vector<FramebufferRenderbufferAttachment> &renderbuffer_attachments = {});
void createFramebuffer(Framebuffer &fb, int width, int height, const std::vector<FramebufferTextureAttachment> &texture_attachments,
                       const std::vector<FramebufferRenderbufferAttachment> &renderbuffer_attachments = {});
void deleteFramebuffer(Framebuffer &fb) noexcept;

inline void uniform(GLint loc, GLint x) {
  glUniform1i(loc, x);
}
inline void uniform(GLint loc, GLuint x) {
  glUniform1ui(loc, x);
}
inline void uniform(GLint loc, GLfloat x) {
  glUniform1f(loc, x);
}
inline void uniform(GLint loc, double x) {
  glUniform1f(loc, static_cast<GLfloat>(x));
}

inline void uniform(GLint loc, float x, float y) {
  glUniform2f(loc, x, y);
}
inline void uniform(GLint loc, const vec2 &v) {
  glUniform2fv(loc, 1, &v.x);
}

inline void uniform(GLint loc, float x, float y, float z) {
  glUniform3f(loc, x, y, z);
}
inline void uniform(GLint loc, const vec3 &v) {
  glUniform3fv(loc, 1, &v.x);
}

inline void uniform(GLint loc, float x, float y, float z, float w) {
  glUniform4f(loc, x, y, z, w);
}
inline void uniform(GLint loc, const vec4 &v) {
  glUniform4fv(loc, 1, &v.x);
}

inline void uniform(GLint loc, const mat2 &m) {
  glUniformMatrix2fv(loc, 1, GL_FALSE, &m[0].x);
}

inline void uniform(GLint loc, const mat3 &m) {
  glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0].x);
}

inline void uniform(GLint loc, const mat4 &m) {
  glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0].x);
}

template <typename T>
void uniform(const Program &prog, const GLchar *name, const T &x) {
  uniform(getUniformLocation(prog, name), x);
}

inline void useProgram(const Program &prog) {
  glUseProgram(prog.id);
}

inline void bindTexture(const Texture &tex) {
  glBindTexture(tex.opts.target, tex.id);
}

inline void bindTexture(const Texture &tex, GLenum tex_unit) {
  glActiveTexture(tex_unit);
  glBindTexture(tex.opts.target, tex.id);
}

void bindFramebuffer(const Framebuffer &fb);

inline void unbindFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void enableBlend(GLenum src_factor, GLenum dest_factor) {
  glEnable(GL_BLEND);
  glBlendFunc(src_factor, dest_factor);
}

inline void disableBlend() {
  glDisable(GL_BLEND);
}

inline void enableBlendAlpha() {
  enableBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

inline void enableBlendAlphaPremult() {
  enableBlend(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

inline void enableBlendAdditive() {
  enableBlend(GL_SRC_ALPHA, GL_ONE);
}

inline void enableBlendScreen() {
  enableBlend(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
}

inline void enableDepth() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
}

inline void disableDepth() {
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
}

} // gl
