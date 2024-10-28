$input v_texcoord0

#include "common.sh"

uniform vec4 u_baseColor;
uniform vec4 u_materialParams; // (metallic, roughness, ior, alpha)

// Function to simulate the wave texture (based on sine wave)
vec3 waveTexture(vec2 uv, float scale, float distortion, float detailScale, float phaseOffset)
{
    float wave = sin(uv.x * scale + distortion * sin(uv.y * detailScale) + phaseOffset);
    return vec3(wave); // Returning as grayscale
}

void main()
{
    vec2 mappedCoords = v_texcoord0;
    
    // Apply scale and rotation
    mappedCoords = mappedCoords * 7.0;  // Scale
    float angle = radians(45.0);
    mat2 rotation = mat2(cos(angle), sin(angle), -sin(angle), cos(angle));
    mappedCoords = mappedCoords * rotation;  // Rotation

    // Simulate wave texture using the transformed coordinates
    vec3 wave1 = waveTexture(mappedCoords, 7.0, 2.0, 1.0, 0.5);
    vec3 wave2 = waveTexture(mappedCoords, 7.0, 2.0, 1.0, 0.0);

    // Color ramp logic (simplified two-stop gradient)
    float rampFactor = clamp(wave1.r, 0.0, 1.0);
    vec3 colorFromRamp = mix(vec3(1.0), vec3(0.0), rampFactor); // Black-to-white ramp

    // Apply Darken operation (min of two colors)
    vec3 darkenedColor = min(colorFromRamp, wave2);

    // Mix base color and darkened color
    vec3 finalColor = mix(u_baseColor.rgb, darkenedColor, 1.0); // Factor 1.0 for full mix

    // Set final fragment color
    gl_FragColor = vec4(finalColor, u_materialParams.a); // Use alpha from u_materialParams
}