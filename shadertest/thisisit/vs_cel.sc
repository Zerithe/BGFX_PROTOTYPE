// Vertex Shader (vs_cel.sc)
$input a_position, a_normal
$output v_normal, v_pos

#include <bgfx_shader.sh>

void main() {
    v_pos = mul(u_model[0], vec4(a_position, 1.0)).xyz;
    v_normal = normalize(mul(u_model[0], vec4(a_normal, 0.0)).xyz);
    gl_Position = mul(u_viewProj, mul(u_model[0], vec4(a_position, 1.0)));
}