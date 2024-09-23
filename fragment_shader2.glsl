#version 330 core

in vec2 TexCoords;    // Passed in from the vertex shader
out vec4 FragColor;   // Output color to the framebuffer

void main() {
    // Simple crosshatching pattern using sine and cosine functions
    float lines1 = abs(sin(10.0 * TexCoords.x + 10.0 * TexCoords.y));
    float lines2 = abs(cos(10.0 * TexCoords.x - 10.0 * TexCoords.y));
    float pattern = lines1 * lines2; // Multiply patterns to increase the hatching effect

    // Use pattern to modulate some base color, for instance, a grayscale tone
    float baseColor = 0.9; // Light gray
    float shade = baseColor * (1.0 - pattern); // Apply hatching pattern
    
    FragColor = vec4(vec3(shade), 1.0);
}
