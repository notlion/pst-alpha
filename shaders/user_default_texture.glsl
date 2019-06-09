void mainTexture(out vec4 fragColor, in vec2 fragCoord, in vec4 baseColor, in vec2 texcoord) {
  fragColor = baseColor;
  fragColor.rgb *= 1.2 * smoothstep(1.0, 0.2, distance(vec2(0.5), gl_FragCoord.xy / iResolution));
  fragColor.rg *= texcoord;
}
