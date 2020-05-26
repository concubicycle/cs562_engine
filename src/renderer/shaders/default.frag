#version 430 core
out vec4 FragColor;

#define EPSILON 0.0000001
#define PI 3.1415926535897932384626433832795
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8


in VS_OUT {
    vec3 normal;
    vec2 texcoords_2d;
    vec3 world_pos;
} fs_in;


// standard surface textures
layout(binding = 0) uniform sampler2D diffuse_texture;
layout(binding = 1) uniform sampler2D metalness_texture;
layout(binding = 2) uniform sampler2D normal_texture;
layout(binding = 3) uniform sampler2D roughness_texture;
layout(binding = 4) uniform sampler2D ambient_occlusion_texture;

// CS541 surface uniforms
uniform vec3 specular;// Ks
uniform float shininess;// alpha exponent

// punctual lights
struct PointLight {
    vec3 color;
    vec3 position;
    float intensity;
    float radius;
};

uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int point_light_count;

// ambient light(s)
uniform vec3 ambient_light;// Ia

// camera uniforms
uniform vec3 camera_position;



vec3 BRDF(vec3 L, vec3 V, vec3 Kd, vec3 Ks, float alpha, PointLight light)
{
    vec3 N = normalize(fs_in.normal);
    vec3 H = normalize(L+V);

    vec3 IaKd = ambient_light * Kd;
    vec3 IiKd = light.color * Kd;
    vec3 IiKs = light.color * Ks;

    float NdotL = dot(N, L);
    if (NdotL < DOT_CLAMP)
        return vec3(0);
    
    NdotL = max(DOT_CLAMP, NdotL);
    float NdotH = max(DOT_CLAMP, dot(N, H));

    float VdotN = max(DOT_CLAMP, dot(V, N));
    float LdotH = max(DOT_CLAMP, dot(L, H));

    float NdotH_alpha = pow(NdotH, alpha);

    // A diffuse light part of BRDF.
    vec3 KdOverPi = Kd / PI;

    // The Schlick approximation to the Fresnel term F
    vec3 F = Ks + (vec3(1)-Ks) * pow(1-LdotH, 5);

    // an approximation to G(L, V, H) / (dot(L,N)*dot(V,N)). G(L, V, H) Represents how
    // much is self-occluded. That is, what percent of microfacet normals reflect light
    // toward another microfacet. I think... might be wrong.
    float Gcheat = 1 / pow(LdotH, 2);

    // The micro-facet normal distribution term D (what part of microfacets
    // reflects in V direction). There's a similarity to blinn phong.
    float D = ((alpha + 2)/(2 * PI)) * NdotH_alpha;

    // KdPi + F(L, H) * G(L, V, H) * D(H) / (4 * ldotn * VdotN)
    vec3 brdf = KdOverPi + F * Gcheat * D / 4;

    // Blinn Phong
    //vec3 I = IaKd + IaKd * NdotL + IiKs * NdotH_alpha;

    return light.color * NdotL * brdf;
}


void main()
{
//    FragColor = vec4(0, 1, 0, 1);
//    return;


    vec4 tex_color = texture(diffuse_texture, fs_in.texcoords_2d);
    vec3 Kd = tex_color.rgb;
    vec3 IaKd = ambient_light * Kd;
    vec3 I = IaKd;

    for (int i = 0; i < point_light_count; i++)
    {
        if (dot(point_lights[i].color, point_lights[i].color) < 0.0001)
        {
            continue;
        }

        vec3 light_pos = point_lights[i].position.xyz;
        vec3 light_vec = light_pos - fs_in.world_pos;
        vec3 view_vec = camera_position.xyz - fs_in.world_pos;
        vec3 V = normalize(view_vec);
        vec3 L = normalize(light_vec);

        float distance_falloff = point_lights[i].intensity / length(light_vec);

        I += BRDF(
            L,
            V,
            Kd,
            specular,
            shininess,
            point_lights[i]) * distance_falloff;
    }    

    if (tex_color.w == 0) discard;    

    FragColor = vec4(I, tex_color.w);
}