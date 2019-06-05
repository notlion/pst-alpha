void mainTexture(out vec4 fragColor, in vec2 fragCoord, in vec4 elemColor) {
  fragColor = elemColor * 1.0 + 0.2 * sin(iTime);
}
