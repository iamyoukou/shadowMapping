#version 330

in vec2 uv;
in vec3 worldPos;
in vec3 worldN;
in vec4 lightSpacePos;

uniform sampler2D texDepth, texScene;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 eyePoint;
uniform int drawScene;

float ka = 0.1, kd = 0.75, ks = 0.25;
float alpha = 20;

out vec4 outputColor;

void main() {
  outputColor = vec4(0);

  if (drawScene == 1) {
    vec3 N = worldN;
    vec3 L = normalize(lightPosition - worldPos);
    vec3 V = normalize(eyePoint - worldPos);
    vec3 H = normalize(L + V);

    vec4 ambient = ka * vec4(lightColor, 0.0);
    vec4 diffuse = kd * vec4(lightColor, 0.0);
    vec4 specular = vec4(lightColor * ks, 1.0);

    float dist = length(L);
    float attenuation = 1.0 / (dist * dist);
    float dc = max(dot(N, L), 0.0);
    float sc = pow(max(dot(H, N), 0.0), alpha);

    outputColor += ambient;
    outputColor += diffuse * dc * attenuation;
    outputColor += specular * sc * attenuation;
  }
  // draw shadow
  else {
    // after [-1, 1] to [0, 1],
    // ndc will be the uv coordinates of the current fragment on the screen
    vec3 ndc = lightSpacePos.xyz / lightSpacePos.w;
    ndc = ndc / 2.0 + 0.5;

    float closestDepth = texture(texDepth, ndc.xy).r;
    float currentDepth = ndc.z;
    bool shadow = closestDepth > currentDepth ? true : false;

    vec4 sceneColor = texture(texScene, ndc.xy);

    // float z = abs(1 - currentDepth);
    // z *= 1000.0;
    // outputColor = vec4(z);

    if (!shadow) {
      outputColor = mix(outputColor, vec4(0), 0.75);
    } else {
      // outputColor = vec4(1.0);
      discard;
    }
  }
}
