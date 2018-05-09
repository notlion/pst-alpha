#include "app/glgeom.hpp"
#include "app/util.hpp"
#include "app/log.hpp"

#include <algorithm>

namespace gl {


const std::vector<VertexAttribute> DefaultVertex::default_attribs{
  { GL_FLOAT, 3, sizeof(DefaultVertex), offsetof(DefaultVertex, position) },
  { GL_FLOAT, 3, sizeof(DefaultVertex), offsetof(DefaultVertex, normal) },
  { GL_FLOAT, 2, sizeof(DefaultVertex), offsetof(DefaultVertex, texcoord) }
};


DefaultTriangleMesh createQuad(const vec2 &min, const vec2 &max, const vec2 &uv_min, const vec2 &uv_max) {
  const DefaultVertex vs[4] = {
    { vec3(min.x, min.y, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(uv_min.x, uv_min.y) },
    { vec3(min.x, max.y, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(uv_min.x, uv_max.y) },
    { vec3(max.x, min.y, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(uv_max.x, uv_min.y) },
    { vec3(max.x, max.y, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(uv_max.x, uv_max.y) }
  };
  return {
    {
      { { vs[0], vs[1], vs[2] } },
      { { vs[1], vs[3], vs[2] } }
    },
    DefaultVertex::default_attribs
  };
}


BoundingBox calcBoundingBox(const DefaultTriangleMesh &mesh) {
  auto box = BoundingBox{ std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
  for (const auto &tri : mesh.triangles) {
    for (const auto &vtx : tri.vertices) {
      box.min = gl::min(box.min, vtx.position);
      box.max = gl::max(box.max, vtx.position);
    }
  }
  return box;
}

vec3 calcCenter(const BoundingBox &box) {
  return 0.5f * box.min + 0.5f * box.max;
}


void createVertexBuffer(VertexBuffer &vb, GLenum primitive, std::size_t data_size, std::size_t count, const void *data, const std::vector<VertexAttribute> &attribs) {
  deleteVertexBuffer(vb);

  glGenBuffers(1, &vb.buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vb.buffer);
  glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

  vb.primitive = primitive;
  vb.count = count;
  vb.attribs = attribs;
}

void deleteVertexBuffer(VertexBuffer &vb) noexcept {
  if (vb.buffer > 0) {
    glDeleteBuffers(1, &vb.buffer);
    vb.buffer = 0;
  }
  vb.count = 0;
}


void enableVertexBuffer(VertexBuffer &vb) {
  glBindBuffer(GL_ARRAY_BUFFER, vb.buffer);

  for (const auto &attr : vb.attribs) {
    if (attr.loc >= 0) {
      glEnableVertexAttribArray(attr.loc);
      switch (attr.component_type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT: {
          glVertexAttribIPointer(attr.loc, attr.component_count, attr.component_type, attr.stride, reinterpret_cast<void *>(attr.offset));
        } break;
        default:
          glVertexAttribPointer(attr.loc, attr.component_count, attr.component_type, GL_FALSE, attr.stride, reinterpret_cast<void *>(attr.offset));
      }
    }
  }

  CHECK_GL_ERROR();
}

void disableVertexBuffer(VertexBuffer &vb) {
  for (const auto &attr : vb.attribs) {
    if (attr.loc >= 0) {
      glDisableVertexAttribArray(attr.loc);
    }
  }

  CHECK_GL_ERROR();
}

void drawVertexBuffer(VertexBuffer &vb) {
  enableVertexBuffer(vb);

  glDrawArrays(vb.primitive, 0, vb.count);

  CHECK_GL_ERROR();

  disableVertexBuffer(vb);
}

void assignVertexBufferAttributeLocations(VertexBuffer &vb, const std::vector<GLint> &attrib_locs) {
  assert(attrib_locs.size() == vb.attribs.size());

  for (std::size_t i = 0; i < vb.attribs.size(); ++i) {
    vb.attribs[i].loc = attrib_locs[i];
  }
}

void assignVertexBufferAttributeLocations(VertexBuffer &vb, const Program &prog, const std::vector<const std::string_view> &attrib_names) {
  assert(attrib_names.size() == vb.attribs.size());

  for (std::size_t i = 0; i < vb.attribs.size(); ++i) {
    vb.attribs[i].loc = gl::getAttribLocation(prog, attrib_names[i].data());
  }
}


VertexBuffer::VertexBuffer(VertexBuffer &&vb) noexcept
: primitive(std::move(vb.primitive)),
  count(std::move(vb.count)),
  attribs(std::move(vb.attribs)) {
  deleteVertexBuffer(*this);
  buffer = vb.buffer;
  vb.buffer = 0;
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&vb) noexcept {
  if (this != &vb) {
    deleteVertexBuffer(*this);
    buffer = vb.buffer;
    
    primitive = std::move(vb.primitive);
    count = std::move(vb.count);
    attribs = std::move(vb.attribs);

    vb.buffer = 0;
  }
  return *this;
}

VertexBuffer::~VertexBuffer() noexcept {
  deleteVertexBuffer(*this);
}

} // gl
