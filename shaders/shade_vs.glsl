#version 300 es

precision highp float;
precision highp int;

// {{vertex}}

// layout(location = 0) in ivec2 aParticleCoord;

void main() {
  mainVertex(gl_Position);//, aParticleCoord);
}
