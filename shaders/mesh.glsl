precision highp float;

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out vec3 v_normal;
out vec2 v_texcoord;

uniform mat4 u_mvp_matrix;
uniform mat3 u_normal_matrix;

void main() {
  v_texcoord = a_texcoord;
  v_normal = normalize(u_normal_matrix * a_normal);

  gl_Position = u_mvp_matrix * vec4(a_position, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER

in vec3 v_normal;
in vec2 v_texcoord;

out vec4 fragColor;

void main() {
  fragColor = vec4(v_texcoord, 0.0, 1.0);
}

#endif
