// https://developer.oculus.com/blog/tech-note-shader-snippets-for-efficient-2d-dithering/
float dither17(vec2 p) {
	vec3 k0 = vec3(2.0, 7.0, 23.0);
	float ret = dot(vec3(p, float(iFrame & 34)), k0 / 17.0);
	return fract(ret);
}

vec2 rotate(vec2 v, float t) {
  return vec2(v.x * cos(t) - v.y * sin(t), v.x * sin(t) + v.y * cos(t));
}

in float vDepth;

void mainFragment(out vec4 oColor, in vec4 color, in vec2 texcoord) {
  vec2 p = texcoord - vec2(0.5);
  float d = length(p);
  p = rotate(p, d * (color.a * 20.0 + 5.0) - iTime * 2.0);
  p.x *= 2.0;
  d = length(p);
  float alpha = smoothstep(0.5, 0.25, d);

  if (alpha < dither17(gl_FragCoord.xy)) discard;
  
  float b = smoothstep(0.8, 0.0, d);
  b = b * b * b * 1.5;
  oColor = vec4(color.rgb * b * (0.25 / vDepth), 1.0);
}
