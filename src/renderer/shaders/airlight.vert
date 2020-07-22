#version 430 core

#define LIGHT_VOLUME_BIAS 0.2

// attribute
layout (location = 0) in vec3 position;

layout(binding = 3) uniform sampler2D shadow_map;

uniform mat4 light_view_inverse;
uniform mat4 light_projection_inverse;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eye_position;

out VS_OUT {
    float view_depth;
    vec4 fragment_position;
} vs_out;

void main()
{
    if (position.z < 0) // this is the light vertex
    {
        // light projection -> world space
        gl_Position =         
            light_view_inverse * 
            light_projection_inverse *
            vec4(position, 1);

        vs_out.fragment_position = gl_Position;

        // world space -> camera view space
        gl_Position = view * gl_Position;

        vs_out.view_depth = -gl_Position .z;
        
        // final NDC output
        gl_Position = projection * gl_Position;
        return;
    }

    vec2 tex_coords = position.xy;
    float depth = texture(shadow_map, tex_coords).x * 100;
    
    // bring to [-1, 1] - NDC
    vec4 pixel = vec4(tex_coords*2.0-1.0, -0.9, 1.0);

    // bring to light view space
    pixel = light_projection_inverse * pixel;

    // orthographic projection - could just set z. add a bit of padding
    pixel.z = -1 * depth + LIGHT_VOLUME_BIAS;
        
    vs_out.fragment_position = light_view_inverse * pixel;
    
    // bias to avoid artifacts. this is not necessary, given a different fixe 
    // presented in Real Time Volumetric Shadows using Polygonal Light Volumes

//    vec3 to_viewer = normalize(eye_position - vs_out.fragment_position.xyz);
//    vs_out.fragment_position +=  vec4(to_viewer * LIGHT_VOLUME_BIAS, 0);

    // camera space
    gl_Position = view * vs_out.fragment_position;
    vs_out.view_depth = -gl_Position .z;
    gl_Position = projection * gl_Position;
}
