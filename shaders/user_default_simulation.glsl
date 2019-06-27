float hash1(uint n) {
	n = (n << 13U) ^ n;
  n = n * (n * n * 15731U + 789221U) + 1376312589U;
  return float( n & uvec3(0x7fffffffU))/float(0x7fffffff);
}

float getDepth(vec2 p) {
  float t0 = iTime * 0.1;
  float d = sin(p.x * 2.653 + t0) * 0.3;
  d += sin(p.y * 1.951 + t0) * 0.3;
  float l = length(p);
  d += 0.3 * smoothstep(0.96, 1.0, abs(fract(l * 0.1 - iTime * 0.1) * 2.0 - 1.0));
  d += l * l * l * 0.008 * cos(12.0 * atan(p.x, p.y) + iTime);
  d *= smoothstep(10.0, 0.0, l);
  return d;
}

void mainSimulation(out vec4 fragPosition, out vec4 fragColor, out vec3 fragRightVector, out vec3 fragUpVector) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int count = int(iResolution.x) * int(iResolution.y);
  float u = float(id) / float(count);
  float r = hash1(uint(id));

  float t = iTime + r * 20.0;
  fragPosition = vec4(gl_FragCoord.xy / iResolution * 7.0 - 3.5, 0.0, 1.0);
  vec2 fp = fragPosition.xy + 0.5 * vec2(cos(t * 0.1), sin(t * 0.1));

  fragPosition.z += getDepth(fp) - 4.0;

  vec2 o = vec2(0.0, 0.01);
  vec3 tx = normalize(vec3(o.xy, getDepth(fp + o.xy) - getDepth(fp - o.xy)));
  vec3 ty = normalize(vec3(o.yx, getDepth(fp + o.yx) - getDepth(fp - o.yx)));
  vec3 normal = cross(tx, ty);

  fragRightVector = tx * r * 0.1;
  fragUpVector = ty * (1.0 - r) * 0.1;

  fragColor.rg = vec2(texcoord) / iResolution;
  fragColor.b = r * 0.4;
  fragColor.rgb *= smoothstep(4.0, 2.0, length(fp)) * 1.5;
  fragColor.a = 1.0;
}
