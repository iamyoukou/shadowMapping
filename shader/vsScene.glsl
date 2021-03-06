#version 330
layout(location = 0) in vec3 vtxCoord;
layout(location = 1) in vec2 texUv;
layout(location = 2) in vec3 vtxN;

out vec2 uv;
out vec3 worldPos;
out vec3 worldN;
out vec4 lightSpacePos;
out vec4 clipSpace;

uniform mat4 M, V, P;
uniform mat4 lightV, lightP;

void main() {
  // projection plane
  gl_Position = P * V * M * vec4(vtxCoord, 1.0);

  uv = texUv;

  worldPos = (M * vec4(vtxCoord, 1.0)).xyz;

  worldN = (vec4(vtxN, 1.0) * inverse(M)).xyz;
  worldN = normalize(worldN);

  lightSpacePos = lightP * lightV * M * vec4(vtxCoord, 1.0);
  clipSpace = gl_Position;
}
