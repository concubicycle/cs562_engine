#version 430 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gBaseColor;
 

// standard surface textures
layout(binding = 0) uniform sampler2D diffuse_texture;
layout(binding = 1) uniform sampler2D metalness_texture;
layout(binding = 2) uniform sampler2D normal_texture;
layout(binding = 3) uniform sampler2D roughness_texture;
layout(binding = 4) uniform sampler2D ambient_occlusion_texture;


in VS_OUT {
    vec4 world_pos;
    vec3 normal;
    vec2 texcoords_2d;    
} fs_in;


void main()
{
    gPosition = fs_in.world_pos;
    gNormal = vec4(fs_in.normal, 1);
    gBaseColor = texture(diffuse_texture, fs_in.texcoords_2d);
}  