#version 330

in vec2 uv;
in vec3 worldPos;
in vec3 worldN;
in vec4 lightSpacePos;
in vec4 clipSpace;

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

  // draw scene without shadow
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

    float shadowFactor = 0.75;

    float closestDepth = texture(texDepth, ndc.xy).r;
    float currentDepth = ndc.z;
    bool isInShadow = currentDepth > closestDepth ? true : false;

    // PCF
    if (isInShadow) {
      vec2 texelSize = 1.0 / textureSize(texDepth, 0);

      // 3x3 samples
      for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
          // skip the original uv
          if (x == 0 && y == 0)
            continue;

          float pcfDepth = texture(texDepth, ndc.xy + vec2(x, y) * texelSize).r;
          bool isShadow = currentDepth > pcfDepth ? true : false;

          if (!isShadow) {
            shadowFactor -= 0.15;
          }
        }
      } // end 3x3 samples

      shadowFactor = max(shadowFactor, 0.0);
    } // // end PCF

    // scene color which is in shadow
    vec2 sceneNdc = clipSpace.xy / clipSpace.w;
    sceneNdc = sceneNdc / 2.0 + 0.5;
    vec4 sceneColor = texture(texScene, sceneNdc);

    // if fragment is in shadow,
    // blend the scene color with the shadow
    if (isInShadow) {
      outputColor = mix(sceneColor, vec4(0), shadowFactor);
    } else {
      discard;
    }
  }
}
