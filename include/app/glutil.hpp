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

#include "glm.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#if defined(NDEBUG)
  #define CHECK_GL_ERROR() ((void)0)
  #define DEBUG_PRINT_GL_STATS() ((void)0)
#else
  #define CHECK_GL_ERROR() gl::checkError()
  #define DEBUG_PRINT_GL_STATS() gl::printStats();
#endif

namespace gl {

using namespace glm;

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

struct UniformBlock {
  GLint binding = -1;

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

struct ShaderError {
  std::string infoLog;
};

struct ProgramError {
  ShaderError vertexShader;
  ShaderError fragmentShader;

  std::string infoLog;
};

struct Program {
  GLuint id = 0;

  std::vector<Uniform> uniforms;
  std::vector<UniformBlock> uniform_blocks;
  std::vector<Attribute> attributes;

  GL_UTIL_MOVE_ONLY_CLASS(Program)
};

struct UniformBuffer {
  GLuint id = 0;

  GL_UTIL_MOVE_ONLY_CLASS(UniformBuffer)
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

void printStats();

GLuint createShader(std::string_view shader_src, GLenum type, ShaderError *error = nullptr);

Program createProgram(std::string_view vert_shader_src, std::string_view frag_shader_src, ProgramError *error = nullptr, bool *success = nullptr);
bool createProgram(Program &prog, std::string_view vert_shader_src, std::string_view frag_shader_src, ProgramError *error = nullptr);
Program createProgram(std::string_view shader_src, ShaderVersion version = SHADER_VERSION_100, ProgramError *error = nullptr);
bool createProgram(Program &prog, std::string_view shader_src, ShaderVersion version = SHADER_VERSION_100);
void deleteProgram(Program &prog) noexcept;

GLint getUniformLocation(const Program &prog, std::string_view name);
GLint getAttribLocation(const Program &prog, std::string_view name);
GLint getUniformBlockIndex(const Program &prog, std::string_view name);

void uniformBlockBinding(Program &prog, std::string_view uniform_block_name, GLuint uniform_block_binding);

void createUniformBuffer(UniformBuffer &ub, std::size_t uniform_data_size_bytes, const void *data, GLenum usage = GL_DYNAMIC_DRAW);
void updateUniformBuffer(UniformBuffer &ub, std::size_t uniform_data_size_bytes, const void *data);
void bindUniformBuffer(UniformBuffer &ub, GLuint uniform_block_binding);
void deleteUniformBuffer(UniformBuffer &ub);

template <typename UniformData>
void createUniformBuffer(UniformBuffer &ub, const UniformData &uniform_data, GLenum usage = GL_STATIC_DRAW) {
  createUniformBuffer(ub, sizeof(UniformData), &uniform_data, usage);
}

template <typename UniformData>
void updateUniformBuffer(UniformBuffer &ub, const UniformData &uniform_data) {
  updateUniformBuffer(ub, sizeof(UniformData), &uniform_data);
}

Texture createTexture(int width, int height, const TextureOpts &opts = {});
Texture createTexture(const TextureData &data, const TextureOpts &opts = {});
void createTexture(Texture &tex, int width, int height, const TextureOpts &opts = {});
void createTexture(Texture &tex, const TextureData &data, const TextureOpts &opts = {});
void deleteTexture(Texture &tex) noexcept;

Renderbuffer createRenderbuffer(int width, int height, const RenderbufferOpts &opts = {});
void createRenderbuffer(Renderbuffer &rb, int width, int height, const RenderbufferOpts &opts = {});
void deleteRenderbuffer(Renderbuffer &rb) noexcept;

Framebuffer createFramebuffer(int width, int height, const std::vector<FramebufferTextureAttachment> &texture_attachments, const std::vector<FramebufferRenderbufferAttachment> &renderbuffer_attachments = {});
void createFramebuffer(Framebuffer &fb, int width, int height, const std::vector<FramebufferTextureAttachment> &texture_attachments, const std::vector<FramebufferRenderbufferAttachment> &renderbuffer_attachments = {});
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

template <GLsizei len>
inline void uniform(GLint loc, const GLint (&data)[len]) {
  glUniform1iv(loc, len, data);
}
template <GLsizei len>
inline void uniform(GLint loc, const GLuint (&data)[len]) {
  glUniform1iv(loc, len, data);
}
template <GLsizei len>
inline void uniform(GLint loc, const GLfloat (&data)[len]) {
  glUniform1iv(loc, len, data);
}

inline void uniform(GLint loc, GLfloat x, GLfloat y) {
  glUniform2f(loc, x, y);
}
inline void uniform(GLint loc, const vec2 &v) {
  glUniform2fv(loc, 1, &v.x);
}

inline void uniform(GLint loc, GLfloat x, GLfloat y, GLfloat z) {
  glUniform3f(loc, x, y, z);
}
inline void uniform(GLint loc, const vec3 &v) {
  glUniform3fv(loc, 1, &v.x);
}

inline void uniform(GLint loc, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  glUniform4f(loc, x, y, z, w);
}
inline void uniform(GLint loc, const vec4 &v) {
  glUniform4fv(loc, 1, &v.x);
}

inline void uniform(GLint loc, GLint x, GLint y) {
  glUniform2i(loc, x, y);
}
inline void uniform(GLint loc, const ivec2 &v) {
  glUniform2iv(loc, 1, &v.x);
}

inline void uniform(GLint loc, GLint x, GLint y, GLint z) {
  glUniform3i(loc, x, y, z);
}
inline void uniform(GLint loc, const ivec3 &v) {
  glUniform3iv(loc, 1, &v.x);
}

inline void uniform(GLint loc, GLint x, GLint y, GLint z, GLint w) {
  glUniform4i(loc, x, y, z, w);
}
inline void uniform(GLint loc, const ivec4 &v) {
  glUniform4iv(loc, 1, &v.x);
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
void uniform(const Program &prog, std::string_view name, const T &x) {
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
