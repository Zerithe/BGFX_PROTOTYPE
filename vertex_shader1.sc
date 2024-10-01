$input a_position vec3
$input a_texcoord0 vec2

#include "bgfx_shader.sh"
#include "varying.def.sc"

// Output data to fragment shader
varying vec2 v_texcoord0;

// Uniforms
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    // Transform the vertex into eye space
    gl_Position = mul(u_projection, mul(u_view, mul(u_model, vec4(a_position, 1.0))));
    // Pass through texture coordinates
    v_texcoord0 = a_texcoord0;
}