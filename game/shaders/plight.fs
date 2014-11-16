#include <shaders/screenspace.h>
#include <shaders/depth.h>
#include <shaders/light.h>

uniform neoSampler2D gColorMap;
uniform neoSampler2D gNormalMap;

uniform pointLight gPointLight;

out vec4 fragColor;

vec4 calcPointLight(vec3 worldPosition, vec3 normal, vec2 spec) {
    vec3 lightDirection = worldPosition - gPointLight.position;
    float distance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    vec4 color = distance < gPointLight.radius
        ? calcLight(gPointLight.base, lightDirection, worldPosition, normal, spec) : vec4(0.0);

    float attenuation = 1.0f - clamp(distance / gPointLight.radius, 0.0f, 1.0f);
    return color * attenuation;
}

void main() {
    vec2 texCoord = calcTexCoord();
    vec4 colorMap = neoTexture2D(gColorMap, texCoord);
    vec4 normalDecode = neoTexture2D(gNormalMap, texCoord);
    vec3 normalMap = normalDecode.rgb * 2.0f - 1.0f;
    vec3 worldPosition = calcPosition(texCoord);
    vec2 specMap = vec2(colorMap.a * 2.0f, exp2(normalDecode.a * 8.0f));

    fragColor = vec4(colorMap.rgb, 1.0f)
        * calcPointLight(worldPosition, normalMap, specMap);
}
