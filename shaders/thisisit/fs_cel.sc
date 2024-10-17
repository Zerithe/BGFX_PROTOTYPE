// Fragment Shader (fs_cel.sc)
$input v_normal, v_pos

#include <bgfx_shader.sh>

uniform vec4 u_lightDir;
uniform vec4 u_lightColor;

void main() {
    vec3 lightDir = normalize(u_lightDir.xyz);
    vec3 normal = normalize(v_normal);
    
    // Calculate diffuse lighting
    float intensity = max(dot(normal, lightDir), 0.0);
    
    // Quantize the intensity to create cel shading effect
    if (intensity > 0.95) {
        intensity = 1.0;
    }
    else if (intensity > 0.5) {
        intensity = 0.6;
    }
    else if (intensity > 0.25) {
        intensity = 0.4;
    }
    else {
        intensity = 0.2;
    }
    
    // Final color
    vec3 finalColor = u_lightColor.rgb * intensity;
    gl_FragColor = vec4(finalColor, 1.0);
}