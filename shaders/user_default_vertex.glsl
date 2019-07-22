void mainVertex(out vec4 oPosition, out vec4 oColor, in vec2 quadPosition, in ivec2 particleCoord) {
  vec4 particlePos = texelFetch(iFragData[0], particleCoord, 0);
  particlePos.xyz += texelFetch(iFragData[2], particleCoord, 0).xyz * quadPosition.x;
  particlePos.xyz += texelFetch(iFragData[3], particleCoord, 0).xyz * quadPosition.y;

  oPosition = iModelViewProjection * particlePos;
  oColor = texelFetch(iFragData[1], particleCoord, 0);
}
