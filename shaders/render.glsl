precision highp float;

#ifdef VERTEX_SHADER

uniform sampler2D iPosition;
uniform sampler2D iColor;

uniform mat4 iModelViewProjection;

layout(location = 0) in ivec2 aTexcoord;

out vec4 vColor;

void main() {
  vColor = texelFetch(iColor, aTexcoord, 0);
  gl_Position = iModelViewProjection * texelFetch(iPosition, aTexcoord, 0);
  gl_PointSize = 2.0;
}

#endif

#ifdef FRAGMENT_SHADER
in vec4 vColor;

out vec4 oFragColor;

void main() {
  oFragColor = vColor;
}
#endif
