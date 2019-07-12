precision highp float;

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

#ifdef VERTEX_SHADER
uniform sampler2D iPosition;
uniform sampler2D iColor;
uniform sampler2D iRight;
uniform sampler2D iUp;

layout(location = 0) in ivec2 aParticleTexcoord;
layout(location = 1) in vec3 aQuadPosition;
layout(location = 2) in vec3 aQuadNormal;
layout(location = 3) in vec2 aQuadTexcoord;

out vec4 vColor;
out vec2 vTexcoord;

void main() {
  vColor = texelFetch(iColor, aParticleTexcoord, 0);
  vTexcoord = aQuadTexcoord;

  vec4 particlePos = texelFetch(iPosition, aParticleTexcoord, 0);
  particlePos.xyz += texelFetch(iRight, aParticleTexcoord, 0).xyz * aQuadPosition.x +
                     texelFetch(iUp, aParticleTexcoord, 0).xyz * aQuadPosition.y;

  gl_Position = iModelViewProjection * particlePos;
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec2 iResolution;

// {texture}

in vec4 vColor;
in vec2 vTexcoord;

out vec4 oFragColor;

void main() {
  mainTexture(oFragColor, gl_PointCoord, vColor, vTexcoord);
}
#endif
