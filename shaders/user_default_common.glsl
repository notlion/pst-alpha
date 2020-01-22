// The contents of this tab will be prefixed in all shaders.

const vec3 cubeVertices[8] = vec3[8](
  vec3(-1.0, -1.0, -1.0),
  vec3( 1.0, -1.0, -1.0),
  vec3(-1.0,  1.0, -1.0),
  vec3( 1.0,  1.0, -1.0),
  vec3(-1.0, -1.0,  1.0),
  vec3( 1.0, -1.0,  1.0),
  vec3(-1.0,  1.0,  1.0),
  vec3( 1.0,  1.0,  1.0)
);

const vec3 cubeNormals[6] = vec3[6](
  vec3( 0.0,  0.0, -1.0),
  vec3( 0.0, -1.0,  0.0),
  vec3(-1.0,  0.0,  0.0),
  vec3( 0.0,  1.0,  0.0),
  vec3( 1.0,  0.0,  0.0),
  vec3( 0.0,  0.0,  1.0)
);

const int cubeIndices[36] = int[36](
  0, 2, 3,
  0, 3, 1,
  0, 1, 5,
  0, 5, 4,
  0, 4, 6,
  0, 6, 2,
  7, 2, 6,
  7, 3, 2,
  7, 1, 3,
  7, 5, 1,
  7, 4, 5,
  7, 6, 4
);
