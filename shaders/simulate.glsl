precision highp float;

#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_position;

void main() {
  gl_Position = a_position;
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D u_position;
uniform sampler2D u_position_prev;
uniform sampler2D u_color;
uniform sampler2D u_color_prev;

uniform ivec2 u_resolution;

layout (std140) uniform SimulationFrameData {
  int u_frame;
  float u_time;
  float u_time_delta;
};

layout(location = 0) out vec4 o_position;
layout(location = 1) out vec4 o_color;

void main() {
  ivec2 texcoord = ivec2(gl_FragCoord);

  vec4 pos = texelFetch(u_position, texcoord, 0);
  vec4 pos_prev = texelFetch(u_position_prev, texcoord, 0);

  vec4 color = texelFetch(u_color, texcoord, 0);
  vec4 color_prev = texelFetch(u_color_prev, texcoord, 0);

  vec3 vel = pos.xyz - pos_prev.xyz;
  vel *= 0.9;

  vec2 uv = gl_FragCoord.xy / vec2(u_resolution);
  o_position = vec4(pos.xyz + vel, 1.0);
  o_color = vec4(sin(u_time + uv.x), cos(u_time + uv.y), 0.0, 1.0);
}

#endif
