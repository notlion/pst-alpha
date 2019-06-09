precision highp float;

#ifdef VERTEX_SHADER
uniform sampler2D iPosition;
uniform sampler2D iColor;
uniform sampler2D iRight;
uniform sampler2D iUp;

uniform mat4 iModelViewProjection;

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
in vec2 vTexcoord;

out vec4 oFragColor;

void main() {
  mainTexture(oFragColor, gl_PointCoord, vColor, vTexcoord);
}
#endif
