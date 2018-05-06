#pragma once

#include "glutil.hpp"

namespace gl {

struct VertexAttribute {
  GLenum component_type = GL_FLOAT;
  GLint component_size = 0;
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

struct TriangleMeshVertexBuffer {
  GLuint buffer = 0;
  GLsizei count = 0;
  std::vector<VertexAttribute> attribs;

  GL_UTIL_MOVE_ONLY_CLASS(TriangleMeshVertexBuffer)
};

struct BoundingBox {
  vec3 min;
  vec3 max;
};

using DefaultTriangleMesh = TriangleMesh<DefaultVertex>;

DefaultTriangleMesh createQuad(const vec2 &min = vec2(-1.0f), const vec2 &max = vec2(1.0f), const vec2 &uv_min = vec2(0.0f), const vec2 &uv_max = vec2(1.0f));

BoundingBox calcBoundingBox(const DefaultTriangleMesh &mesh);
vec3 calcCenter(const BoundingBox &box);

void createTriangleMeshVertexBuffer(TriangleMeshVertexBuffer &vb, std::size_t triangle_size, std::size_t triangle_count, const void *data, const std::vector<VertexAttribute> &attribs);
void deleteTriangleMeshVertexBuffer(TriangleMeshVertexBuffer &vb) noexcept;

template <typename Vertex>
void createTriangleMeshVertexBuffer(TriangleMeshVertexBuffer &vb, const TriangleMesh<Vertex> &mesh) {
  createTriangleMeshVertexBuffer(vb, sizeof(Triangle<Vertex>), mesh.triangles.size(), mesh.triangles.data(), mesh.attribs);
}

template <typename Vertex>
TriangleMeshVertexBuffer createTriangleMeshVertexBuffer(const TriangleMesh<Vertex> &mesh) {
  TriangleMeshVertexBuffer vb;
  createTriangleMeshVertexBuffer(vb, mesh);
  return vb;
}

void enableTriangleMeshVertexBuffer(TriangleMeshVertexBuffer &vb);
void disableTriangleMeshVertexBuffer(TriangleMeshVertexBuffer &vb);

void assignTriangleMeshVertexBufferAttributeLocations(TriangleMeshVertexBuffer &vb, const std::vector<GLint> &attrib_locs);
void assignTriangleMeshVertexBufferAttributeLocations(TriangleMeshVertexBuffer &vb, const Program &prog, const std::vector<const std::string_view> &attrib_names);

void drawTriangleMeshVertexBuffer(TriangleMeshVertexBuffer &vb);

} // gl
