precision highp float;
precision highp int;

#ifdef VERTEX_SHADER
layout(location = 0) in vec4 aPosition;

void main() {
  gl_Position = aPosition;
}
#endif

#ifdef FRAGMENT_SHADER
uniform sampler2D iFragData0;
uniform sampler2D iPrevFragData0;
uniform sampler2D iFragData1;
uniform sampler2D iPrevFragData1;
uniform sampler2D iFragData2;
uniform sampler2D iPrevFragData2;
uniform sampler2D iFragData3;
uniform sampler2D iPrevFragData3;

layout(std140) uniform CommonUniforms {
  mat4 iModelViewProjection;
  mat4 iModelView;
  mat4 iProjection;
  mat4 iInverseModelViewProjection;
  mat4 iInverseModelView;
  mat4 iInverseProjection;

  mat4 iControllerTransform[2]; // [Left, Right]
  vec4 iControllerVelocity[2];
  vec4 iControllerButtons[2];

  float iTime;
  float iTimeDelta;
  float iFrame;
};

uniform vec2 iResolution;

// {simulation}

layout(location = 0) out vec4 oFragData0;
layout(location = 1) out vec4 oFragData1;
layout(location = 2) out vec4 oFragData2;
layout(location = 3) out vec4 oFragData3;

void main() {
  mainSimulation(oFragData0, oFragData1, oFragData2, oFragData3);
}
#endif
