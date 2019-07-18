precision highp float;
precision highp int;

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

#ifdef VERTEX_SHADER
void mainVertex(out vec4 oPosition, out vec4 oColor, in vec2 quadPosition, in ivec2 particleCoord) {
  vec4 particlePos = texelFetch(iFragData0, particleCoord, 0);
  particlePos.xyz += texelFetch(iFragData2, particleCoord, 0).xyz * quadPosition.x;
  particlePos.xyz += texelFetch(iFragData3, particleCoord, 0).xyz * quadPosition.y;

  oPosition = iModelViewProjection * particlePos;
  oColor = texelFetch(iFragData1, particleCoord, 0);
}

layout(location = 0) in vec2 aQuadPosition;
layout(location = 1) in vec2 aQuadTexcoord;
layout(location = 2) in ivec2 aParticleCoord;

out vec4 vColor;
out vec2 vTexcoord;

void main() {
  vTexcoord = aQuadTexcoord;
  mainVertex(gl_Position, vColor, aQuadPosition, aParticleCoord);
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec2 iResolution;

// {texture}

in vec4 vColor;
in vec2 vTexcoord;

out vec4 oFragColor;

void main() {
  mainFragment(oFragColor, vColor, vTexcoord);
}
#endif
