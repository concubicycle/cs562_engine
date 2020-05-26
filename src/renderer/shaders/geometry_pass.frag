#version 430 core
layout (location = 0) out vec4 gPosition;
 

// gbuffer output
//layout (location = 0) out vec3 gPosition;
//layout (location = 1) out vec3 gNormal;
//layout (location = 2) out vec3 gBaseColor;

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



void main()
{
//    gPosition = vec3(0, 1, 0); //fs_in.world_pos;    
//    gNormal = vec3(0, 0, 1); // normalize(fs_in.normal);
//    gBaseColor = vec3(1, 0, 0); //texture(diffuse_texture, fs_in.texcoords_2d).rgb;

    gPosition = vec4(fs_in.world_pos, 1);
}  