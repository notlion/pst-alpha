precision highp float;

#ifdef VERTEX_SHADER

layout(location = 0) in vec4 aPosition;

void main() {
  gl_Position = aPosition;
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D iPosition;
uniform sampler2D iPositionPrev;
uniform sampler2D iColor;
uniform sampler2D iColorPrev;

uniform vec2  iResolution;
uniform int   iFrame;
uniform float iTime;
uniform float iTimeDelta;

// {simulation}

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oColor;

void main() {
  mainSimulation(oPosition, oColor);
}

#endif
