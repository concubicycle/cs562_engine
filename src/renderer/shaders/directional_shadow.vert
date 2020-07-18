#version 430 core

// standard vertex attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 color;


out VS_OUT {
    float depth;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{    
    vec4 world_pos = model * vec4(position, 1);
    vec4 view_pos = view * world_pos;
    gl_Position = projection * view * world_pos;
    vs_out.depth = -view_pos.z/100;
}

