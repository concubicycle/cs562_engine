#version 430 core

const int MAX_BONES = 128;

// standard vertex attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 color;

layout (location = 6) in ivec4 bone_ids;
layout (location = 7) in vec4 weights;

layout(location = 45) uniform mat4 u_bones[MAX_BONES];


out VS_OUT {
    float depth;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{    
     mat4 bone_matrix =
		u_bones[bone_ids.x] * weights.x +
		u_bones[bone_ids.y] * weights.y +
		u_bones[bone_ids.z] * weights.z +
		u_bones[bone_ids.w] * weights.w;

    vec4 position_local = bone_matrix * vec4(position, 1);
    vec4 world_pos = model * position_local;
    
    vec4 view_pos = view * world_pos;
    gl_Position = projection * view * world_pos;
    vs_out.depth = -view_pos.z/200;
}

