vec3 hash3(uint n) {
  n = (n << 13U) ^ n;
  n = n * (n * n * 15731U + 789221U) + 1376312589U;
  uvec3 k = n * uvec3(n, n * 16807U, n * 48271U);
  return vec3(k & uvec3(0x7fffffffU)) / float(0x7fffffff);
}

void mainSimulation(out vec4 oPosition, out vec4 oColor, out vec4 oRight, out vec4 oUp, out vec4 oPrevPos, out vec4 oExtra) {
  ivec2 texcoord = ivec2(gl_FragCoord);
  int id = (texcoord.x + texcoord.y * iResolution.x);

  if (iFrame == 0 || iControllerButtons[1][2] > 0.0) {
    vec3 r0 = hash3(uint(id));
    vec3 r1 = hash3(uint(id + 1));
    vec3 r2 = hash3(uint(id + 2));
    
    oPosition = vec4(r0 * 0.25 - 0.125, 1.0);
    oPosition.z -= 1.0;
    oPrevPos = oPosition;
    oPrevPos.xyz -= (r2 - 0.5) * 0.01;
    oRight = vec4(normalize(r1), 1.0);
    oUp = vec4(normalize(cross(oRight.xyz, r2)), 1.0);
    float scale = 0.18 + r1.x * 0.15;
    scale *= scale * scale;
    oRight *= scale;
    oUp *= scale;
    oColor.xyz = r0;
    oExtra = vec4(r1, 0.0);
  }
  else {
    vec4 pos = texelFetch(iFragData[0], texcoord, 0);
    vec4 ppos = texelFetch(iFragData[4], texcoord, 0);

    vec4 vel = pos - ppos;
    vel *= 0.995;

    oExtra = texelFetch(iFragData[5], texcoord, 0);

    for (int i = 0; i < 2; ++i) {
      float b1 = iControllerButtons[i][1];
      if (b1 > 0.0) {
        vec4 o = iControllerTransform[i][3] - pos;
        vel += o / length(o) * 0.001 * b1 * b1 * b1;
      }
    }

    oPosition = pos + vel;
    oPrevPos = pos;

    oColor = texelFetch(iFragData[1], texcoord, 0);
    oRight = texelFetch(iFragData[2], texcoord, 0);
    oUp = texelFetch(iFragData[3], texcoord, 0);
  }
}
