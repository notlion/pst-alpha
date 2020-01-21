#pragma size 128 128

void mainSimulation(out vec4 oPosition, out vec4 oColor, out vec4 oData2, out vec4 oData3, out vec4 oData4, out vec4 oData5) {
  ivec2 coord = ivec2(gl_FragCoord);
  int id = iSize.x * coord.y + coord.x;

  float scale = 1.0 / float(max(iSize.x, iSize.y));
  vec2 pos = (gl_FragCoord.xy - vec2(iSize) * 0.5) * scale;
  oPosition = vec4(pos, -1.5, 1.0);

  vec2 texcoord = gl_FragCoord.xy / vec2(iSize);
  oColor = vec4(texcoord, 0.0, 1.0);
}
