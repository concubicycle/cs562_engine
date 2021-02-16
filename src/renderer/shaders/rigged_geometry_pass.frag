#version 430 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gBaseColor;
layout (location = 3) out vec4 gFresnelColorRoughness;


// standard surface textures
layout(binding = 0) uniform sampler2D diffuse_texture;
layout(binding = 1) uniform sampler2D metalness_texture;
layout(binding = 2) uniform sampler2D normal_texture;
layout(binding = 3) uniform sampler2D roughness_texture;
layout(binding = 4) uniform sampler2D ambient_occlusion_texture;



uniform vec3 fresnel_color;
uniform float roughness;
uniform bool metalness;

uniform bool has_diffuse_texture;
uniform vec4 diffuse_color;

in VS_OUT {
    vec4 world_pos;
    vec3 normal;
    vec2 texcoords_2d;
    float depth;
} fs_in;


void main()
{
    gPosition = vec4(fs_in.world_pos.xyz, metalness ? 1 : 0);
    gNormal = vec4(fs_in.normal, fs_in.depth);
    gBaseColor = has_diffuse_texture ? texture(diffuse_texture, fs_in.texcoords_2d) : diffuse_color;
    gFresnelColorRoughness = vec4(fresnel_color, roughness);
}
