$input v_texcoord0 vec2

#include "bgfx_shader.sh"
#include "varying.def.sc"

// Output data
OUTPUT vec4 o_color;

// Uniforms
uniform float u_time;

// Function to generate wave patterns
float wavePattern(float bands, float phase, float width) {
    return step(cos(bands * v_texcoord0.x + phase), width);
}

void main() {
    // Create two wave patterns with different phases
    float wave1 = wavePattern(20.0, u_time, 0.5);
    float wave2 = wavePattern(20.0, u_time + 3.14, 0.5);
    
    // Combine wave patterns using a maximum function to mimic mixing
    float combined = max(wave1, wave2);
    
    // Output color
    o_color = vec4(vec3(combined), 1.0);
}