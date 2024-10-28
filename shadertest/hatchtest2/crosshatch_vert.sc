// Declare the inputs and outputs for the vertex shader
$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord0, v_lightDir

#include "common.sh"  // Ensure common utilities and declarations are included

// Uniforms for transformations and lighting
uniform mat4 u_modelViewProj;
uniform mat4 u_model;
uniform vec4 u_lightPos;

void main()
{
    // Transform vertex position
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

    // Pass through the texture coordinates
    v_texcoord0 = a_texcoord0;

    // Calculate normal in world space
    vec3 worldNormal = normalize((u_model * vec4(a_normal, 0.0)).xyz);
    v_normal = worldNormal;

    // Calculate light direction in world space
    vec3 lightDir = normalize(u_lightPos.xyz - (u_model * vec4(a_position, 1.0)).xyz);
    v_lightDir = lightDir;
}