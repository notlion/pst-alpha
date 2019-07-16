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
             smin(distance(p, vec3(0.15, 0.7, -2.2)) - 0.3,
                  smin(distance(p, vec3(-0.2, 1.0, -2.0)) - 0.35,
                       distance(p, vec3(0.0, 1.5, -2.1)) - 0.2,
                       0.2),
                  0.2));
}

void mainSimulation(out vec4 oPos, out vec4 oColor, out vec4 oRight, out vec4 oUp, out vec4 oPrevPos, out vec4 oUnused) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * int(iResolution.x));
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
    int f = int(iFrame) + i;
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
      oPos = texelFetch(iFragData0, texcoord, 0);
      oColor = texelFetch(iFragData1, texcoord, 0);
      oRight = texelFetch(iFragData2, texcoord, 0) * 0.95;
      oUp = texelFetch(iFragData3, texcoord, 0) * 0.95;
    }
  }
  // Photons
  else {
    int count = int(iResolution.x * iResolution.y) - 148;
    int i = id - 148;
    int f = int(iFrame) + i;
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
      vec4 pos = texelFetch(iFragData0, texcoord, 0);
      vec4 prevPos = texelFetch(iFragData4, texcoord, 0);
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
        oColor = texelFetch(iFragData1, texcoord, 0);
        oPos = pos;
        oPrevPos = pos;
      }
      else {
        oPos = pos;
        oColor = texelFetch(iFragData1, texcoord, 0);
        oUp = oPos - oPrevPos;
        oRight = vec4(0.05 * cross(oUp.xyz, up), 0.0);
      }
    }
  }
}
