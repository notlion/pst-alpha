#version 300 es

precision highp float;
precision highp int;

// {{simulation}}

layout(location = 0) out vec4 oFragData0;
layout(location = 1) out vec4 oFragData1;
layout(location = 2) out vec4 oFragData2;
layout(location = 3) out vec4 oFragData3;
layout(location = 4) out vec4 oFragData4;
layout(location = 5) out vec4 oFragData5;

void main() {
  mainSimulation(oFragData0, oFragData1, oFragData2, oFragData3, oFragData4, oFragData5);
}
