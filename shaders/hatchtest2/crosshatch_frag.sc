$input v_normal, v_texcoord0, v_lightDir

#include "common.sh"  

// Uniforms for cross-hatching textures
uniform vec4 u_hatchParams[5];

// Sampler for the hatch textures
uniform sampler2D u_hatch1;
uniform sampler2D u_hatch2;
uniform sampler2D u_hatch3;
uniform sampler2D u_hatch4;
uniform sampler2D u_hatch5;

void main()
{
    // Normalize interpolated normal and light direction
    vec3 N = normalize(v_normal);
    vec3 L = normalize(v_lightDir);

    // Calculate light intensity
    float intensity = dot(N, L);

    // Select the appropriate hatch texture based on intensity
    vec4 color;
    if (intensity > 0.8)
    {
        color = texture2D(u_hatch1, v_texcoord0);
    }
    else if (intensity > 0.6)
    {
        color = texture2D(u_hatch2, v_texcoord0);
    }
    else if (intensity > 0.4)
    {
        color = texture2D(u_hatch3, v_texcoord0);
    }
    else if (intensity > 0.2)
    {
        color = texture2D(u_hatch4, v_texcoord0);
    }
    else
    {
        color = texture2D(u_hatch5, v_texcoord0);
    }

    // Output the final color
    gl_FragColor = color;
}