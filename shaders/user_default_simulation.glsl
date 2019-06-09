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

void mainSimulation(out vec4 fragPosition, out vec4 fragColor, out vec3 fragRightVector, out vec3 fragUpVector) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int count = int(iResolution.x) * int(iResolution.y);
  float u = float(id) / float(count);

  fragPosition = vec4(gl_FragCoord.xy / iResolution * 7.0 - 3.5, 0.0, 1.0);
  vec2 fp = fragPosition.xy + 0.5 * vec2(cos(iTime * 0.1), sin(iTime * 0.1));
  fragPosition.z = getDepth(fp);

  vec2 o = vec2(0.0, 0.01);
  vec3 tx = normalize(vec3(o.xy, getDepth(fp + o.xy) - getDepth(fp - o.xy)));
  vec3 ty = normalize(vec3(o.yx, getDepth(fp + o.yx) - getDepth(fp - o.yx)));
  vec3 normal = cross(tx, ty);

  fragRightVector = tx * 0.005;
  fragUpVector = ty * 0.005;

  float d = length(fp);
  vec3 c0 = mix(vec3(147, 165, 0) / 255.0, vec3(139, 43, 21) / 255.0, d * 0.4);
  vec3 c1 = mix(vec3(36, 38, 33) / 255.0, vec3(36, 58, 122) / 255.0, d * 0.3);
  c1 = mix(vec3(132, 26, 27) / 255.0, c1, smoothstep(0.0, 1.0, d));
  fragColor.rgb = mix(c0, c1, smoothstep(0.4, 0.6, abs(fract(d * 7.0 - iTime) * 2.0 - 1.0)));
  fragColor.a = 1.0;

  vec3 ld = normalize(vec3(-1.0, -1.0, -2.0));
  fragColor.rgb *= mix(0.4, 1.5, max(0.0, dot(ld, normal)));
}
