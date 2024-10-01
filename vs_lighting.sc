$input a_position vec3
$input a_normal vec3

#include "common.sh"

// Output to fragment shader
varying vec3 v_position;
varying vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    vec4 worldPosition = mul(u_model, vec4(a_position, 1.0));

    // Pass the world position and normal to the fragment shader
    v_position = worldPosition.xyz;
    v_normal = a_normal;

    // Set the vertex position
    gl_Position = mul(u_projection, mul(u_view, worldPosition));
}