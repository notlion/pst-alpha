## Particle ShaderToy Samples

### Schuyff Waves
```
// Yet more IQ https://www.shadertoy.com/view/ll2GD3
vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
  return a + b * cos(6.28318 * (c * t + d));
}

float getDepth(vec2 p) {
  float d = sin(p.x * 2.653 + iTime) * 0.3;
  d += sin(p.y * 1.951 + iTime) * 0.3;
  float l = length(p);
  d += 0.2 * smoothstep(0.98, 1.0, abs(fract(l * 0.05 - iTime * 0.1) * 2.0 - 1.0));
  d += l * l * l * 0.008 * cos(12.0 * atan(p.x, p.y) + iTime);
  d *= smoothstep(10.0, 0.0, l);
  return d;
}

void mainSimulation(out vec4 fragPosition, out vec4 fragColor) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int count = int(iResolution.x) * int(iResolution.y);
  float u = float(id) / float(count);

  fragPosition = vec4(gl_FragCoord.xy / iResolution * 7.0 - 3.5, 0.0, 1.0);
  vec2 fp = fragPosition.xy + 0.5 * vec2(cos(iTime * 0.1), sin(iTime * 0.1));
  fragPosition.z = getDepth(fp);

  vec2 o = vec2(0.0, 0.01);
  vec3 normal = normalize(cross(
    vec3(o.xy, getDepth(fp + o.xy) - getDepth(fp - o.xy)),
    vec3(o.yx, getDepth(fp + o.yx) - getDepth(fp - o.yx))));

  float d = length(fp);
  vec3 c0 = mix(vec3(147, 165, 0) / 255.0, vec3(139, 43, 21) / 255.0, d * 0.4);
  vec3 c1 = mix(vec3(36, 38, 33) / 255.0, vec3(36, 58, 122) / 255.0, d * 0.3);
  c1 = mix(vec3(132, 26, 27) / 255.0, c1, smoothstep(0.0, 1.0, d));
  fragColor.rgb = mix(c0, c1, smoothstep(0.4, 0.6, abs(fract(d * 7.0 - iTime) * 2.0 - 1.0)));
  fragColor.a = 1.0;

  vec3 ld = normalize(vec3(-1.0, -1.0, -2.0));
  fragColor.rgb *= mix(0.4, 1.5, max(0.0, dot(ld, normal)));
}
```

### Orb Fountain
```
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
  int rate = 2000;
  int frame = (iFrame - id / rate) % (count / rate);

  vec3 pos = texelFetch(iPosition, texcoord, 0).xyz;
  vec3 pos_prev = texelFetch(iPositionPrev, texcoord, 0).xyz;

  vec3 vel = (pos - pos_prev) * 0.99;

  if (frame == 0) {
    fragPosition = vec4(0.8, 0.25, 0.0, 1.0);
  }
  else if (frame == 1) {
    vel = 0.01 * (hash33(uvec3(iFrame, texcoord.x, texcoord.y)) - 0.5);
    float t = sin(iTime) * 6.28318 * 2.0;
    vel += vec3(sin(t) - 1.5, 2.0, cos(t)) * mix(0.01, 0.03, hash12(uvec2(iFrame, id)));
    fragPosition = vec4(pos + vel, 1.0);
  }
  else {
    float r = sin(iTime * 2.0) * 0.05 + 1.4;
    if (length(pos) > r) pos = r * normalize(pos);
    vel.y -= 0.002;
    fragPosition = vec4(pos + vel, 1.0);
  }

  float u = float(id) / float(count);
  fragColor = vec4(pal(u, vec3(0.5), vec3(0.5), vec3(1.0), vec3(0.0, 0.33, 0.67)), 1.0);
  fragColor.rgb *= min(1.0, length(vel) * 20.0);
}
```
