precision highp float;
precision highp int;

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
uniform mat4  iModelViewProjection;
uniform mat4  iModelView;
uniform mat4  iProjection;
uniform mat4  iInverseModelViewProjection;
uniform mat4  iInverseModelView;
uniform mat4  iInverseProjection;
uniform int   iFrame;
uniform float iTime;
uniform float iTimeDelta;

// {simulation}

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oColor;
layout(location = 2) out vec4 oRightVector;
layout(location = 3) out vec4 oUpVector;

void main() {
  mainSimulation(oPosition, oColor, oRightVector.xyz, oUpVector.xyz);
}
#endif
