## Particle ShaderToy Samples

### Schuyff Waves
```
// Yet more IQ https://www.shadertoy.com/view/ll2GD3
vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
  return a + b * cos(6.28318 * (c * t + d));
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

  fragPosition = vec4(gl_FragCoord.xy / iResolution * 7.0 - 3.5, 0.0, 1.0);
  vec2 fp = fragPosition.xy + 0.5 * vec2(cos(iTime * 0.1), sin(iTime * 0.1));
  fragPosition.z = getDepth(fp) - 3.0;

  vec2 o = vec2(0.0, 0.01);
  vec3 tx = normalize(vec3(o.xy, getDepth(fp + o.xy) - getDepth(fp - o.xy)));
  vec3 ty = normalize(vec3(o.yx, getDepth(fp + o.yx) - getDepth(fp - o.yx)));
  vec3 normal = cross(tx, ty);

  fragRightVector = tx * 0.01;
  fragUpVector = ty * 0.01;

  float d = length(fp);
  vec3 c0 = mix(vec3(147, 165, 0) / 255.0, vec3(139, 43, 21) / 255.0, d * 0.4);
  vec3 c1 = mix(vec3(36, 38, 33) / 255.0, vec3(36, 58, 122) / 255.0, d * 0.3);
  c1 = mix(vec3(132, 26, 27) / 255.0, c1, smoothstep(0.0, 1.0, d));
  fragColor.rgb = mix(c0, c1, smoothstep(0.4, 0.6, abs(fract(d * 7.0 - iTime * 0.5) * 2.0 - 1.0)));
  fragColor.a = 1.0;

  vec3 ld = normalize(vec3(-1.0, -1.0, -2.0));
  fragColor.rgb *= mix(0.4, 1.5, max(0.0, dot(ld, normal)));
}
```

### Orb Fountain
```
// IQ Integer Hash https://www.shadertoy.com/view/XlXcW4
vec3 hash33(uvec3 x) {
  for (int i = 0; i < 3; ++i) x = ((x >> 8U) ^ x.yzx) * 1103515245U;
  return vec3(x) * (1.0 / float(0xffffffffU));
}
float hash12(uvec2 x) {
  uvec2 q = 1103515245U * ((x >> 1U) ^ (x.yx));
  uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
  return float(n) * (1.0 / float(0xffffffffU));
}

// IQ Cosine palette https://www.shadertoy.com/view/ll2GD3
vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
  return a + b * cos(6.28318 * (c * t + d));
}

// Branchless orthogonal vector http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
vec3 orthogonal(vec3 v) {
  float k = fract(abs(v.x) + 0.5);
  return vec3(-v.y, v.x - k * v.z, k * v.y);
}

void mainSimulation(out vec4 fragPosition, out vec4 fragColor, out vec3 fragRightVector, out vec3 fragUpVector) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int count = int(iResolution.x) * int(iResolution.y);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
  int rate = 200;
  int frame = (iFrame - id / rate) % (count / rate);

  vec3 pos = texelFetch(iPosition, texcoord, 0).xyz;
  vec3 pos_prev = texelFetch(iPositionPrev, texcoord, 0).xyz;

  float r = sin(iTime * 2.0) * 0.05 + 1.3;
  float d = length(pos) - r;
  vec3 n = normalize(pos);

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
    if (d > 0.0) pos = r * n;
    vel.y -= 0.002;
    fragPosition = vec4(pos + vel, 1.0);
  }

  if (frame == 0) {
    fragRightVector = vec3(0.0);
    fragUpVector = vec3(0.0);
  }
  else {
    vec3 z = normalize(vel);
    vec3 y = orthogonal(z);

    fragRightVector = vel * 0.4;
    fragUpVector = y * 0.004;
  }

  float u = float(id) / float(count);
  fragColor = vec4(pal(u, vec3(0.5), vec3(0.5), vec3(1.0), vec3(0.0, 0.33, 0.67)), 1.0);
  fragColor.rgb *= min(1.0, length(vel) * 20.0);

  vec3 lightDir = normalize(vec3(8.0, 3.0, 2.0));
  fragColor.rgb *= mix(1.0, 1.0 + dot(n, lightDir), smoothstep(-1.0, 0.0, d));
}
```

### Shattered Fields
```
const vec3 cubeFaceNormals[6] = vec3[6](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0));

float hash1(uint n) {
  n = (n << 13U) ^ n;
  n = n * (n * n * 15731U + 789221U) + 1376312589U;
  return float(n & uvec3(0x7fffffffU)) / float(0x7fffffff);
}

vec3 hash3(uint n) {
  n = (n << 13U) ^ n;
  n = n * (n * n * 15731U + 789221U) + 1376312589U;
  uvec3 k = n * uvec3(n, n * 16807U, n * 48271U);
  return vec3(k & uvec3(0x7fffffffU)) / float(0x7fffffff);
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

void mainSimulation(out vec4 oPosition, out vec4 oColor, out vec4 oRight, out vec4 oUp, out vec4 oUnused0, out vec4 oUnused1) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * iResolution.x);
  float r = hash1(uint(id));

  if (id < 12) {
    mat4 xf = iControllerTransform[id / 6];
    vec3 scale = vec3(0.025, 0.025, 0.1);
    oPosition = xf * vec4(cubeFaceNormals[id % 6] * scale, 1.0);
    oColor = vec4(vec3(r * 0.75 + 0.25), 1.0);
    oRight = xf * vec4(cubeFaceNormals[(id + 1) % 6] * scale, 0.0);
    oUp = xf * vec4(cubeFaceNormals[(id + 2) % 6] * scale, 0.0);
  }
  else if (id < 20) {
    int i = id - 12;
    int controllerId = i / 4;
    int buttonId = i % 4;
    mat4 xf = iControllerTransform[controllerId];
    oPosition = xf * vec4(0.0, 0.026, 0.022 * float(buttonId), 1.0);
    oColor = vec4(0.4 + 0.6 * iControllerButtons[controllerId][buttonId], 0.0, 0.0, 1.0);
    oRight = xf * vec4(0.01, 0.0, 0.0, 0.0);
    oUp = xf * vec4(0.0, 0.0, 0.009, 0.0);
  }
  else if (id < 20 + 128) {
    int i = id - 20;
    int f = iFrame + i;
    int age = f & 127;
    if (age == 0) {
      mat4 xf = iControllerTransform[i & 1];
      vec3 rv = hash3(uint(i));
      vec3 scale = vec3(0.06, 0.06, 0.21);
      oPosition = xf * vec4((rv - 0.5) * scale, 1.0);
      oColor = vec4(rv, 1.0);
      oRight = xf * vec4(0.02, 0.0, 0.0, 0.0);
      oUp = xf * vec4(0.0, 0.0, 0.02, 0.0);
    }
    else {
      oPosition = texelFetch(iFragData[0], texcoord, 0);
      oColor = texelFetch(iFragData[1], texcoord, 0);
      oRight = texelFetch(iFragData[2], texcoord, 0) * 0.95;
      oUp = texelFetch(iFragData[3], texcoord, 0) * 0.95;
    }
  }
  else {
    float t = iTime + r * 20.0;
    vec2 xy = gl_FragCoord.xy / vec2(iResolution) * 7.0 - 3.5;
    vec2 fp = xy + 0.5 * vec2(cos(t * 0.1), sin(t * 0.1));

    oPosition = vec4(xy.x, 0.0, xy.y, 1.0);
    oPosition.y += getDepth(fp) - 1.0;
    oPosition.z -= 4.0;

    for (int i = 0; i < 2; ++i) {
      vec3 o = oPosition.xyz - iControllerTransform[i][3].xyz;
      oPosition.xyz -= smoothstep(1.0, 0.0, length(o)) * iControllerButtons[i][0] * 2.0 * o;
    }

    vec2 o = vec2(0.0, 0.01);
    vec3 tx = normalize(vec3(o.xy, getDepth(fp + o.xy) - getDepth(fp - o.xy)));
    vec3 ty = normalize(vec3(o.yx, getDepth(fp + o.yx) - getDepth(fp - o.yx)));

    oRight.xyz = tx.xzy * r * 0.1 + 0.005;
    oUp.xyz = ty.xzy * (1.0 - r) * 0.1 + 0.005;

    oColor.rg = vec2(texcoord) / vec2(iResolution);
    oColor.b = r * 0.4;
    float controllerDist = min(distance(oPosition.xyz, iControllerTransform[0][3].xyz), distance(oPosition.xyz, iControllerTransform[1][3].xyz));
    oColor.rgb *= smoothstep(4.0, 2.0, length(fp)) * 1.5 * smoothstep(0.0, 1.0, controllerDist);
    oColor.a = 1.0;
  }
}
```

### Scanner Darkly
```
const vec3 cubeFaceNormals[6] = vec3[6](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0));

float hash1(uint n) {
  n = (n << 13U) ^ n;
  n = n * (n * n * 15731U + 789221U) + 1376312589U;
  return float(n & uvec3(0x7fffffffU)) / float(0x7fffffff);
}
vec3 hash3(uint n) {
  n = (n << 13U) ^ n;
  n = n * (n * n * 15731U + 789221U) + 1376312589U;
  uvec3 k = n * uvec3(n, n * 16807U, n * 48271U);
  return vec3(k & uvec3(0x7fffffffU)) / float(0x7fffffff);
}

float smin(float d1, float d2, float k) {
  float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
  return mix(d2, d1, h) - k * h * (1.0 - h);
}

float map(vec3 p) {
  return min(3.0 - distance(p, vec3(0.0, 1.0, 0.0)),
             smin(distance(p, vec3(0.15, -0.3, -1.4)) - 0.3,
                  smin(distance(p, vec3(-0.2, 0.0, -1.3)) - 0.35,
                       distance(p, vec3(0.0, 0.5, -1.4)) - 0.2,
                       0.2),
                  0.2));
}

void mainSimulation(out vec4 oPos, out vec4 oColor, out vec4 oRight, out vec4 oUp, out vec4 oPrevPos, out vec4 oUnused) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * iResolution.x);
  float r = hash1(uint(id));

  // Controller Body
  if (id < 12) {
    mat4 xf = iControllerTransform[id / 6];
    vec3 scale = vec3(0.025, 0.025, 0.1);
    oPos = xf * vec4(cubeFaceNormals[id % 6] * scale, 1.0);
    oColor = vec4(vec3(r * 0.75 + 0.25), 1.0);
    oRight = xf * vec4(cubeFaceNormals[(id + 1) % 6] * scale, 0.0);
    oUp = xf * vec4(cubeFaceNormals[(id + 2) % 6] * scale, 0.0);
  }
  // Controller Buttons
  else if (id < 20) {
    int i = id - 12;
    int controllerId = i / 4;
    int buttonId = i % 4;
    mat4 xf = iControllerTransform[controllerId];
    oPos = xf * vec4(0.0, 0.026, 0.022 * float(buttonId), 1.0);
    oColor = vec4(0.4 + 0.6 * iControllerButtons[controllerId][buttonId], 0.0, 0.0, 1.0);
    oRight = xf * vec4(0.01, 0.0, 0.0, 0.0);
    oUp = xf * vec4(0.0, 0.0, 0.009, 0.0);
  }
  // Controller Sparkles
  else if (id < 148) {
    int i = id - 20;
    int f = iFrame + i;
    int age = f & 127;
    if (age == 0) {
      mat4 xf = iControllerTransform[i & 1];
      vec3 rv = hash3(uint(i));
      vec3 scale = vec3(0.06, 0.06, 0.21);
      oPos = xf * vec4((rv - 0.5) * scale, 1.0);
      oColor = vec4(rv, 1.0);
      oRight = xf * vec4(0.02, 0.0, 0.0, 0.0);
      oUp = xf * vec4(0.0, 0.0, 0.02, 0.0);
    }
    else {
      oPos = texelFetch(iFragData[0], texcoord, 0);
      oColor = texelFetch(iFragData[1], texcoord, 0);
      oRight = texelFetch(iFragData[2], texcoord, 0) * 0.95;
      oUp = texelFetch(iFragData[3], texcoord, 0) * 0.95;
    }
  }
  // Photons
  else {
    int count = iResolution.x * iResolution.y - 148;
    int i = id - 148;
    int f = iFrame + i;
    int age = f % (count / 16);

    const vec3 up = vec3(0.0, 1.0, 0.0);

    if (age == 0) {
      if (iControllerButtons[i & 1].x > 0.0) {
        vec3 rv = hash3(uint(i));
        mat4 xf = iControllerTransform[i & 1];
        oColor = vec4(rv, 1.0);
        oPos = xf * vec4(rv.xy * 0.04 - 0.02, -0.15 - 0.05 * rv.z, 1.0);
        oPrevPos = xf * vec4(0.0, 0.0, 0.0, 1.0);
        oUp = oPos - oPrevPos;
        oRight = vec4(0.05 * cross(oUp.xyz, up), 0.0);
      }
      else {
        oPos = vec4(0.0);
        oPrevPos = vec4(0.0);
        oUp = vec4(0.0);
        oRight = vec4(0.0);
      }
    }
    else {
      vec4 pos = texelFetch(iFragData[0], texcoord, 0);
      vec4 prevPos = texelFetch(iFragData[4], texcoord, 0);
      oPrevPos = pos;

      vec4 vel = pos - prevPos;

      float d = map(pos.xyz);
      float speed = length(vel);
      if (d > 0.0 && d < speed) {
        vec3 dir = normalize(vel.xyz);
        float len = 0.0;
        for (int i = 0; i < 8; ++i) {
          if (d < 0.01) {
            break;
          }
          pos.xyz += dir * d;
          len += d;
          if (len > speed) {
            pos.xyz += dir * (speed - len);
            break;
          }
          d = map(pos.xyz);
        }
      }
      else {
        pos += vel;
      }

      if (d < 0.01) {
        const vec2 o = vec2(0.01, 0.0);
        vec3 g = normalize(vec3(map(pos.xyz + o.xyy) - d, map(pos.xyz + o.yxy) - d, map(pos.xyz + o.yyx) - d));
        oRight = vec4(0.02 * normalize(cross(up, g)), 0.0);
        oUp = vec4(cross(g, oRight.xyz), 0.0);
        oColor = texelFetch(iFragData[1], texcoord, 0);
        oPos = pos;
        oPrevPos = pos;
      }
      else {
        oPos = pos;
        oColor = texelFetch(iFragData[1], texcoord, 0);
        oUp = oPos - oPrevPos;
        oRight = vec4(0.05 * cross(oUp.xyz, up), 0.0);
      }
    }
  }
}
```
