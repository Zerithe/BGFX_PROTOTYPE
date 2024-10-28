$input v_normal, v_pos

#include <bgfx_shader.sh>

uniform vec4 u_lightDir;        // Light direction
uniform vec4 u_lightColor;      // Light color
uniform sampler2D u_hatchTex;   // Hatching texture

void main() {
    // Normalizing light direction and vertex normal
    vec3 lightDir = normalize(u_lightDir.xyz);
    vec3 normal = normalize(v_normal);
    
    // Calculate diffuse lighting
    float intensity = max(dot(normal, lightDir), 0.0);
    
    // Quantize intensity to create cel shading effect
    if (intensity > 0.95) {
        intensity = 1.0;
    }
    else if (intensity > 0.5) {
        intensity = 0.6;
    }
    else if (intensity > 0.25) {
        intensity = 0.4;
    }
    else {
        intensity = 0.2;
    }

    // ** Crosshatch Effect **

    // UV scaling based on object size (can tweak scaling factor as needed)
    vec2 uv = v_pos.xy * 0.05;

    // Sample the base hatch pattern from the texture
    float hatch1 = texture2D(u_hatchTex, uv).r;

    // Sample a second hatch pattern with a rotated UV for diagonal lines
    float hatch2 = texture2D(u_hatchTex, uv.yx).r;  // Rotated coordinates for diagonal pattern

    // Combine both hatch patterns
    float combinedHatch = min(hatch1, hatch2);  // Ensure the darker (hatch) lines are more prominent
    
    // Adjust the crosshatch based on light intensity
    float hatchFactor = mix(1.0, combinedHatch, 1.0 - intensity);
    
    // Combine cel-shaded color and crosshatch intensity
    vec3 finalColor = u_lightColor.rgb * intensity * hatchFactor;

    // Output final color
    gl_FragColor = vec4(finalColor, 1.0);
}