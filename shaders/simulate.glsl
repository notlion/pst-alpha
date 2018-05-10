precision highp float;

#ifdef VERTEX_SHADER

layout(location = 0) in vec4 aPosition;

void main() {
  gl_Position = aPosition;
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D iPosition;
uniform sampler2D iPositionPrev;
uniform sampler2D iColor;
uniform sampler2D iColorPrev;

uniform vec2  iResolution;
uniform int   iFrame;
uniform float iTime;
uniform float iTimeDelta;

// mainSimulation
void mainSimulation(out vec4 fragPosition, out vec4 fragColor) {
  ivec2 texcoord = ivec2(gl_FragCoord);

  vec4 pos = texelFetch(iPosition, texcoord, 0);
  vec4 pos_prev = texelFetch(iPositionPrev, texcoord, 0);

  vec4 color = texelFetch(iColor, texcoord, 0);
  vec4 color_prev = texelFetch(iColorPrev, texcoord, 0);

  vec3 vel = pos.xyz - pos_prev.xyz;
  vel *= 0.9;

  vec3 color_vel = color.rgb - color_prev.rgb;
  color_vel *= 0.9;

  vec2 uv = gl_FragCoord.xy / vec2(iResolution);

  float z = sin(iTime + uv.x * 5.231) + cos(iTime + uv.y * 5.763);
  z *= 0.5;
  vec3 goal_position = vec3(uv * 2.0 - 1.0, z);
  vec3 goal_color = vec3(sin(z * 10.0) * 0.5 + 0.5, cos(z * 10.0) * 0.5 + 0.5, 0.0);

  vel += (goal_position - pos.xyz) * 0.1;
  color_vel += (goal_color - color.rgb) * 0.1;

  fragPosition = vec4(pos.xyz + vel, 1.0);
  fragColor = vec4(color.rgb + color_vel, 1.0);
}

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oColor;

void main() {
  mainSimulation(oPosition, oColor);
}

#endif
