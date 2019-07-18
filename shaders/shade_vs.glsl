#version 300 es

precision highp float;
precision highp int;

uniform vec2 iResolution;

// {{vertex}}

layout(location = 0) in vec2 aQuadPosition;
layout(location = 1) in vec2 aQuadTexcoord;
layout(location = 2) in ivec2 aParticleCoord;

out vec4 vColor;
out vec2 vTexcoord;

void main() {
  vTexcoord = aQuadTexcoord;
  mainVertex(gl_Position, vColor, aQuadPosition, aParticleCoord);
}
