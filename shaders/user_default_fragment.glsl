void mainFragment(out vec4 oColor, in vec4 color, in vec2 texcoord) {
  oColor = color;
  oColor.rg *= 0.85 + 0.15 * texcoord;
}
