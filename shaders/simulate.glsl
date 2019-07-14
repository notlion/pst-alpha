precision highp float;
precision highp int;

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

layout(std140) uniform CommonUniforms {
  mat4 iModelViewProjection;
  mat4 iModelView;
  mat4 iProjection;
  mat4 iInverseModelViewProjection;
  mat4 iInverseModelView;
  mat4 iInverseProjection;

  vec4 iControllerPosition[2]; // [Left, Right]
  vec4 iControllerVelocity[2];

  float iTime;
  float iTimeDelta;
  float iFrame;
};

uniform vec2 iResolution;

// {simulation}

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oColor;
layout(location = 2) out vec4 oRightVector;
layout(location = 3) out vec4 oUpVector;

void main() {
  mainSimulation(oPosition, oColor, oRightVector.xyz, oUpVector.xyz);
}
#endif
