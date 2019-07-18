uniform sampler2D iFragData0;
uniform sampler2D iFragData1;
uniform sampler2D iFragData2;
uniform sampler2D iFragData3;
uniform sampler2D iFragData4;
uniform sampler2D iFragData5;

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
