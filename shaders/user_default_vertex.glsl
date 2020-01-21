#pragma vertexCount 6

const vec2 quadVertices[6] = vec2[6](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

out vec4 vColor;

void mainVertex(out vec4 oPosition) {
  int instanceID = gl_VertexID / 6;
  ivec2 coord = ivec2(instanceID % iSize.x, instanceID / iSize.x);

  oPosition = texelFetch(iFragData[0], coord, 0);
  oPosition.xy += quadVertices[gl_VertexID % 6] * 0.0015;

  oPosition = iModelViewProjection * oPosition;

  vColor = texelFetch(iFragData[1], coord, 0);
}
