#include <shaders/fog.h>

in vec3 texCoord0;
in vec3 worldPos0;

out vec3 fragColor;

uniform samplerCube gCubemap;
uniform vec3 gSkyColor;

void main() {
#ifdef USE_FOG
    vec3 color = texture(gCubemap, texCoord0).rgb;
    vec3 cameraToWorld = texCoord0 - worldPos0;
    vec3 eyeDir = normalize(cameraToWorld);

    // Vertical gradient below certain height so that world fog "reaches" into
    // the sky.
    float vertFogGrad = 1.0f - clamp(dot(-eyeDir, vec3(0.0f, 1.0f, 0.0f)) - 0.1f, 0.0f, 0.25f) / 0.25f;
    vec3 verticalMixture = mix(color, gFog.color, vertFogGrad);
    fragColor = mix(verticalMixture, gSkyColor, gFog.density);
#else
    fragColor = texture(gCubemap, texCoord0).rgb;
#endif
}
