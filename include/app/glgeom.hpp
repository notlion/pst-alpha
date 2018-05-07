#pragma once

#include "glutil.hpp"

namespace gl {

struct VertexAttribute {
  GLenum component_type = GL_FLOAT;
  GLint component_count = 0;
  GLsizei stride = 0;
  GLsizeiptr offset = 0;
  GLint loc = -1;
};

struct DefaultVertex {
  static const std::vector<VertexAttribute> default_attribs;
  
  vec3 position;
  vec3 normal;
  vec2 texcoord;
};

template <typename Vertex>
struct Triangle {
  Vertex vertices[3];
};

template <typename Vertex>
struct TriangleMesh {
  std::vector<Triangle<Vertex>> triangles;
  std::vector<VertexAttribute> attribs;
};

struct VertexBuffer {
  GLuint buffer = 0;

  GLenum primitive;
  GLsizei count;

  std::vector<VertexAttribute> attribs;

  GL_UTIL_MOVE_ONLY_CLASS(VertexBuffer)
};

struct BoundingBox {
  vec3 min;
  vec3 max;
};

using DefaultTriangleMesh = TriangleMesh<DefaultVertex>;

DefaultTriangleMesh createQuad(const vec2 &min = vec2(-1.0f), const vec2 &max = vec2(1.0f), const vec2 &uv_min = vec2(0.0f), const vec2 &uv_max = vec2(1.0f));

BoundingBox calcBoundingBox(const DefaultTriangleMesh &mesh);
vec3 calcCenter(const BoundingBox &box);

void createVertexBuffer(VertexBuffer &vb, GLenum primitive, std::size_t triangle_size, std::size_t triangle_count, const void *data, const std::vector<VertexAttribute> &attribs);
void deleteVertexBuffer(VertexBuffer &vb) noexcept;

template <typename Vertex>
void createVertexBuffer(VertexBuffer &vb, const TriangleMesh<Vertex> &mesh) {
  createVertexBuffer(vb, GL_TRIANGLES, sizeof(Triangle<Vertex>) * mesh.triangles.size(), mesh.triangles.size() * 3, mesh.triangles.data(), mesh.attribs);
}

template <typename Vertex>
VertexBuffer createVertexBuffer(const TriangleMesh<Vertex> &mesh) {
  VertexBuffer vb;
  createVertexBuffer(vb, mesh);
  return vb;
}

void enableVertexBuffer(VertexBuffer &vb);
void disableVertexBuffer(VertexBuffer &vb);

void assignVertexBufferAttributeLocations(VertexBuffer &vb, const std::vector<GLint> &attrib_locs);
void assignVertexBufferAttributeLocations(VertexBuffer &vb, const Program &prog, const std::vector<const std::string_view> &attrib_names);

void drawVertexBuffer(VertexBuffer &vb);

} // gl
