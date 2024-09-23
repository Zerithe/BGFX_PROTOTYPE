#version 330 core

// Interpolated values from the vertex shaders
in vec2 TexCoords;

// Ouput data
out vec4 FragColor;

// Uniform values
uniform float time;

// Function to generate wave patterns
float wavePattern(float bands, float phase, float width) {
    return step(cos(bands * TexCoords.x + phase), width);
}

void main() {
    // Create two wave patterns with different phases
    float wave1 = wavePattern(20.0, time, 0.5);
    float wave2 = wavePattern(20.0, time + 3.14, 0.5);
    
    // Combine wave patterns using a maximum function to mimic mixing
    float combined = max(wave1, wave2);
    
    // Output color
    FragColor = vec4(vec3(combined), 1.0);
}
