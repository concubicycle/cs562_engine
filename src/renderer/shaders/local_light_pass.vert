#version 430 core

layout (location = 0) in vec3 position;

out VS_OUT {
    vec4 world_position;
} vs_out;

// camera uniforms
layout (location = 0) uniform mat4 projection;
layout (location = 1) uniform mat4 view;

// model uniforms
layout (location = 2) uniform mat4 model;

void main()
{
    vec4 world_pos = model * vec4(position, 1);
    vs_out.world_position = world_pos;    
    gl_Position = projection * view * world_pos;
}
