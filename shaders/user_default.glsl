// Integer Hash by IQ https://www.shadertoy.com/view/XlXcW4
vec3 hash33(uvec3 x) {
  for (int i = 0; i < 3; ++i) x = ((x >> 8U) ^ x.yzx) * 1103515245U;
  return vec3(x) * (1.0 / float(0xffffffffU));
}
float hash12(uvec2 x) {
  uvec2 q = 1103515245U * ((x >> 1U) ^ (x.yx));
  uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
  return float(n) * (1.0 / float(0xffffffffU));
}

// Yet more IQ https://www.shadertoy.com/view/ll2GD3
vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
  return a + b * cos(6.28318 * (c * t + d));
}

void mainSimulation(out vec4 fragPosition, out vec4 fragColor) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int count = int(iResolution.x) * int(iResolution.y);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int frame = (iFrame - id / 256) % (count / 256);

  vec3 pos = texelFetch(iPosition, texcoord, 0).xyz;
  vec3 pos_prev = texelFetch(iPositionPrev, texcoord, 0).xyz;

  vec3 vel = (pos - pos_prev) * 0.99;

  if (frame == 0) {
    fragPosition = vec4(0.5, 0.25, 0.0, 1.0);
  }
  else if (frame == 1) {
    vel = 0.01 * (hash33(uvec3(iFrame, texcoord.x, texcoord.y)) - 0.5);
    float t = sin(iTime) * 6.28318 * 2.0;
    vel += vec3(sin(t) - 1.5, 2.0, cos(t)) * mix(0.01, 0.03, hash12(uvec2(iFrame, id)));
    fragPosition = vec4(pos + vel, 1.0);
  }
  else {
    float r = sin(iTime * 2.0) * 0.05 + 1.0;
    if (length(pos) > r) pos = r * normalize(pos);
    vel.y -= 0.002;
    fragPosition = vec4(pos + vel, 1.0);
  }

  float u = float(id) / float(count);
  fragColor = vec4(pal(u, vec3(0.5), vec3(0.5), vec3(1.0), vec3(0.0, 0.33, 0.67)), 1.0);
  fragColor.rgb *= min(1.0, length(vel) * 30.0);
}
