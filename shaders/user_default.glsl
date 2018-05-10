// Integer Hash by IQ https://www.shadertoy.com/view/XlXcW4
vec3 hash(uvec3 x) {
  for (int i = 0; i < 3; ++i) x = ((x >> 8U) ^ x.yzx) * 1103515245U;
  return vec3(x) * (1.0 / float(0xffffffffU));
}

void mainSimulation(out vec4 fragPosition, out vec4 fragColor) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int count = int(iResolution.x) * int(iResolution.y);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int frame = (iFrame - id / 128) % (count / 128);

  vec3 pos = texelFetch(iPosition, texcoord, 0).xyz;
  vec3 pos_prev = texelFetch(iPositionPrev, texcoord, 0).xyz;

  vec3 vel = (pos - pos_prev) * 0.99;

  if (frame == 0) {
    fragPosition = vec4(0.0, 0.5, 0.0, 1.0);
  }
  else if (frame == 1) {
    vec3 dir = normalize(hash(uvec3(iFrame, texcoord.x, texcoord.y)) - 0.5);
    fragPosition = vec4(pos + dir * 0.015, 1.0);
  }
  else {
    if (pos.y < -0.5) vel.y *= -0.8;
    else vel.y -= 0.002;
    pos += vel;
    fragPosition = vec4(pos, 1.0);
  }

  fragColor = vec4(vec2(texcoord) / iResolution, 1.0, 1.0);
}