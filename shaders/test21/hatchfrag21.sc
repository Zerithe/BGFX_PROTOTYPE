$input v_normal, v_pos

#include <bgfx_shader.sh>

uniform vec4 u_lightDir;        // Light direction
uniform vec4 u_lightColor;      // Light color
uniform sampler2D u_hatchTex;   // Hatching texture

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Creates a more organic, sketchy line
float sketchyLine(vec2 uv, float angle, float frequency, float thickness) {
    // Rotate UV coordinates
    float c = cos(angle);
    float s = sin(angle);
    vec2 rotatedUV = vec2(
        uv.x * c - uv.y * s,
        uv.x * s + uv.y * c
    );
    
    // Add some randomness to make lines look hand-drawn
    float noise = random(rotatedUV * 0.5) * 0.3;
    float line = sin(rotatedUV.x * frequency + noise);
    
    // Create harder edges with smoothstep
    return smoothstep(thickness, -thickness, abs(line));
}

void main() {
    // Normalizing light direction and vertex normal
    vec3 lightDir = normalize(u_lightDir.xyz);
    vec3 normal = normalize(v_normal);
    
    // Calculate diffuse lighting
    float intensity = max(dot(normal, lightDir), 0.0);
    
    // UV coordinates for hatching
    vec2 uv = v_pos.xy * 3.0; // Adjusted scale for better line density
    
    // Create multiple layers of hatching with different angles and frequencies
    float hatch = 0.0;
    
    if (intensity < 0.75) {
        // First layer - diagonal lines
        hatch += sketchyLine(uv, 0.785, 6.0, 0.1);
    }
    
    if (intensity < 0.5) {
        // Second layer - perpendicular diagonal lines
        hatch += sketchyLine(uv, -0.785, 6.0, 0.1);
    }
    
    if (intensity < 0.25) {
        // Third layer - horizontal lines
        hatch += sketchyLine(uv, 0.0, 8.0, 0.08);
    }
    
    if (intensity < 0.15) {
        // Fourth layer - vertical lines
        hatch += sketchyLine(uv, 1.571, 8.0, 0.08);
    }
    
    // Add some noise to make it look more hand-drawn
    float noise = random(uv) * 0.1;
    
    // Combine everything
    float finalHatch = clamp(hatch + noise, 0.0, 1.0);
    
    // Invert and adjust contrast for a more pencil-like look
    finalHatch = 1.0 - (finalHatch * 0.8);
    
    // Final color combines base color with hatching
    vec3 finalColor = u_lightColor.rgb * finalHatch;
    
    // Output final color
    gl_FragColor = vec4(finalColor, 1.0);