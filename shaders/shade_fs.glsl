#version 300 es

precision highp float;
precision highp int;

// {{fragment}}

out vec4 oFragColor;

void main() {
  mainFragment(oFragColor);
}
