$input v_position vec3
$input v_normal vec3

#include "common.sh"

// Output color
OUTPUT vec4 o_color;

// Light uniforms
uniform vec3 u_lightPos;    // Light position in world space
uniform vec3 u_lightColor;  // Light color
uniform vec3 u_viewPos;     // Camera position

uniform vec3 u_objectColor; // Object color

void main() {
    // Ambient lighting
    vec3 ambient = 0.1 * u_lightColor;

    // Diffuse lighting
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // Specular lighting (Phong reflection)
    vec3 viewDir = normalize(u_viewPos - v_position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);  // Specular exponent (shininess)
    vec3 specular = spec * u_lightColor;

    // Combine results
    vec3 lighting = (ambient + diffuse + specular) * u_objectColor;

    o_color = vec4(lighting, 1.0);
}