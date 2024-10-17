$input a_position, a_texcoord0
$output v_texcoord0

#include "common.sh"

void main()
{
    // Pass texture coordinates to the fragment shader
    v_texcoord0 = a_texcoord0;

    // Calculate the final vertex position using the model-view-projection matrix
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}