#pragma vertexCount 36
#pragma cull back

out vec4 vColor;

void mainVertex(out vec4 oPosition) {
  int instanceID = gl_VertexID / 36;
  ivec2 coord = ivec2(instanceID % iSize.x, instanceID / iSize.x);

  oPosition = texelFetch(iFragData[0], coord, 0);
  oPosition.xyz += cubeVertices[cubeIndices[gl_VertexID % 36]] * 0.004;

  oPosition = iModelViewProjection * oPosition;

  vColor = texelFetch(iFragData[1], coord, 0);

  vec3 normal = cubeNormals[gl_VertexID / 6 % 6];
  vec3 lightDir = normalize(vec3(0.6, 0.3, 1.0));
  vColor *= 0.6 + 0.4 * max(0.0, dot(lightDir, normal));
}
