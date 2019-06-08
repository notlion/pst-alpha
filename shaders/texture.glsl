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
