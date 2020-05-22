#version 330 core

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
    float fog_factor;
} vs_out;

uniform mat4 model, model_inverse, projection, view;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{
    vec4 world_pos = model * vec4(position, 1);

    vs_out.world_pos = world_pos.xyz;
    vs_out.normal = mat3(model_inverse) * normal;
    vs_out.texcoords_2d = texture_coords;

    gl_Position = projection * view * world_pos;
    float maxf = 3000;
    float fd = gl_Position.w > maxf ? maxf : gl_Position.w;
    vs_out.fog_factor = (maxf - fd) / maxf;
}

