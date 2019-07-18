#version 300 es

precision highp float;
precision highp int;

uniform vec2 iResolution;

// {{fragment}}

in vec4 vColor;
in vec2 vTexcoord;

out vec4 oFragColor;

void main() {
  mainFragment(oFragColor, vColor, vTexcoord);
}
