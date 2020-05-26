#version 430 core

// standard vertex attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 color;

out VS_OUT {
    vec3 normal;
    vec2 texcoords_2d;
    vec3 world_pos;
} vs_out;

// camera uniforms
uniform mat4     
    projection, 
    view;

// model uniforms
uniform mat4 
    model,
    adjoint_transpose;

void main()
{
    vec4 world_pos = model * vec4(position, 1);

    vs_out.world_pos = world_pos.xyz;
    vs_out.normal = mat3(adjoint_transpose) * normal;
    vs_out.texcoords_2d = texture_coords;

    gl_Position = projection * view * world_pos;
}

