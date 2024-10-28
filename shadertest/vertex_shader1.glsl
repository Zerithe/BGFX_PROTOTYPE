#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 aPos; // Position
layout (location = 1) in vec2 aTexCoords; // Texture coordinates

// Output data to fragment shader
out vec2 TexCoords;

// Values that stay constant for the whole mesh.
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform the vertex into eye space.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // Pass through texture coordinates
    TexCoords = aTexCoords;
}
