#version 430 core

// gbuffer output
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gBaseColor;

// standard surface textures
layout(binding = 0) uniform sampler2D diffuse_texture;
layout(binding = 1) uniform sampler2D metalness_texture;
layout(binding = 2) uniform sampler2D normal_texture;
layout(binding = 3) uniform sampler2D roughness_texture;
layout(binding = 4) uniform sampler2D ambient_occlusion_texture;


in VS_OUT {
    vec3 normal;
    vec2 texcoords_2d;
    vec3 world_pos;
} fs_in;


in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;


void main()
{
    gPosition = fs_in.world_pos;    
    gNormal =  normalize(fs_in.normal);
    gBaseColor =  texture(diffuse_texture, fs_in.texcoords_2d).rgb;
}  