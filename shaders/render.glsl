precision highp float;

#ifdef VERTEX_SHADER

uniform sampler2D u_position;
uniform sampler2D u_color;

uniform mat4 u_mvp_matrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in ivec2 a_texcoord;

out vec4 v_color;

void main() {
  v_color = texelFetch(u_color, a_texcoord, 0);
  gl_Position = u_mvp_matrix * texelFetch(u_position, a_texcoord, 0);
  gl_PointSize = 4.0;
}

#endif

#ifdef FRAGMENT_SHADER
in vec4 v_color;

out vec4 o_fragcolor;

void main() {
  o_fragcolor = v_color;
}
#endif
