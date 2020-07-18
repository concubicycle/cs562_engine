#version 430 core

// attribute
layout (location = 0) in vec3 position;

layout(binding = 0) uniform sampler2D shadow_map;

uniform mat4 light_view_inverse;
uniform mat4 light_projection_inverse;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    float optical_thickness;
} vs_out;

void main()
{
    if (position.z < 0) // this is the light vertex
    {
        gl_Position = 
            projection *
            view *
            light_view_inverse * 
            light_projection_inverse *
            vec4(position, 1);

        return;
    }

    vec2 tex_coords = position.xy;
    float depth = texture(shadow_map, tex_coords).x * 100;
    vs_out.optical_thickness = depth;

    // bring to [-1, 1] - NDC
    vec4 pixel = vec4(tex_coords*2.0-1.0, 0.5, 1.0);

    // bring to light view space
    pixel = light_projection_inverse * pixel;

    // adjust depth in light view space
    //pixel.xyz = depth * (pixel.xyz / pixel.z);

    pixel.z = -1 * depth;

    // world space
    vec4 world_position = light_view_inverse * pixel;

    // camera space
    gl_Position = projection * view * world_position;
}
