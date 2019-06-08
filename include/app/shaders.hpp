// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !! THIS FILE IS AUTOMATICALLY GENERATED. EDITS WILL BE CLOBBERED !!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#pragma once

const char *shader_source_mesh = R"GLSL(precision highp float;

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

uniform sampler2D u_texture;

void main() {
  fragColor = vec4(texture(u_texture, v_texcoord).rgb, 1.0);
}

#endif
)GLSL";

const char *shader_source_simulate = R"GLSL(precision highp float;

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
uniform mat4  iModelViewProjection;
uniform mat4  iModelView;
uniform mat4  iProjection;
uniform mat4  iInverseModelViewProjection;
uniform mat4  iInverseModelView;
uniform mat4  iInverseProjection;
uniform int   iFrame;
uniform float iTime;
uniform float iTimeDelta;

// {simulation}

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oColor;

void main() {
  mainSimulation(oPosition, oColor);
}

#endif
)GLSL";

const char *shader_source_texture = R"GLSL(precision highp float;

#ifdef VERTEX_SHADER

uniform sampler2D iPosition;
uniform sampler2D iColor;

uniform mat4 iModelViewProjection;

layout(location = 0) in ivec2 aTexcoord;

out vec4 vColor;

void main() {
  vColor = texelFetch(iColor, aTexcoord, 0);
  gl_Position = iModelViewProjection * texelFetch(iPosition, aTexcoord, 0);
  gl_PointSize = 3.0;
}

#endif

#ifdef FRAGMENT_SHADER
uniform vec2  iResolution;
uniform mat4  iModelViewProjection;
uniform mat4  iModelView;
uniform mat4  iProjection;
uniform mat4  iInverseModelViewProjection;
uniform mat4  iInverseModelView;
uniform mat4  iInverseProjection;
uniform int   iFrame;
uniform float iTime;
uniform float iTimeDelta;

// {texture}

in vec4 vColor;

out vec4 oFragColor;

void main() {
  mainTexture(oFragColor, gl_PointCoord, vColor);
}
#endif
)GLSL";

const char *shader_source_user_default_simulation = R"GLSL(// Yet more IQ https://www.shadertoy.com/view/ll2GD3
vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
  return a + b * cos(6.28318 * (c * t + d));
}

float getDepth(vec2 p) {
  float d = sin(p.x * 2.653 + iTime) * 0.3;
  d += sin(p.y * 1.951 + iTime) * 0.3;
  float l = length(p);
  d += 0.2 * smoothstep(0.98, 1.0, abs(fract(l * 0.05 - iTime * 0.1) * 2.0 - 1.0));
  d += l * l * l * 0.008 * cos(12.0 * atan(p.x, p.y) + iTime);
  d *= smoothstep(10.0, 0.0, l);
  return d;
}

void mainSimulation(out vec4 fragPosition, out vec4 fragColor) {//, out vec3 fragRight, out vec3 fragUp) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int count = int(iResolution.x) * int(iResolution.y);
  float u = float(id) / float(count);

  fragPosition = vec4(gl_FragCoord.xy / iResolution * 7.0 - 3.5, 0.0, 1.0);
  vec2 fp = fragPosition.xy + 0.5 * vec2(cos(iTime * 0.1), sin(iTime * 0.1));
  fragPosition.z = getDepth(fp);

  vec2 o = vec2(0.0, 0.01);
  vec3 normal = normalize(cross(
    vec3(o.xy, getDepth(fp + o.xy) - getDepth(fp - o.xy)),
    vec3(o.yx, getDepth(fp + o.yx) - getDepth(fp - o.yx))));

  float d = length(fp);
  vec3 c0 = mix(vec3(147, 165, 0) / 255.0, vec3(139, 43, 21) / 255.0, d * 0.4);
  vec3 c1 = mix(vec3(36, 38, 33) / 255.0, vec3(36, 58, 122) / 255.0, d * 0.3);
  c1 = mix(vec3(132, 26, 27) / 255.0, c1, smoothstep(0.0, 1.0, d));
  fragColor.rgb = mix(c0, c1, smoothstep(0.4, 0.6, abs(fract(d * 7.0 - iTime) * 2.0 - 1.0)));
  fragColor.a = 1.0;

  vec3 ld = normalize(vec3(-1.0, -1.0, -2.0));
  fragColor.rgb *= mix(0.4, 1.5, max(0.0, dot(ld, normal)));
}
)GLSL";

const char *shader_source_user_default_texture = R"GLSL(void mainTexture(out vec4 fragColor, in vec2 fragCoord, in vec4 baseColor) {
  fragColor = baseColor;
  fragColor.rgb *= 1.2 * smoothstep(1.0, 0.2, distance(vec2(0.5), gl_FragCoord.xy / iResolution));
}
)GLSL";
