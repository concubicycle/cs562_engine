#version 430 core
out vec4 FragColor;

#define EPSILON 0.0000001
#define PI 3.1415926535897932384626433832795
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gBaseColor;

in vec2 TexCoords;

// CS541 surface uniforms
uniform vec3 specular;// Ks
uniform float shininess;// alpha exponent

// punctual lights
struct PointLight {
    vec3 color;
    vec3 position;
    float intensity;
    float radius;
    mat4 light_view;
    mat4 light_view_back;
};


uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform sampler2D shadow_maps[MAX_POINT_LIGHTS];
uniform int point_light_count;



// ambient light(s)
uniform vec3 ambient_light;// Ia

// camera uniforms
uniform vec3 camera_position;


vec3 BRDF(vec3 L, vec3 V, vec3 Kd, vec3 Ks, float alpha, PointLight light)
{
    vec3 N = normalize(texture(gNormal, TexCoords).xyz);
    vec3 H = normalize(L+V);    

    vec3 IaKd = ambient_light * Kd;
    vec3 IiKd = light.color * Kd;
    vec3 IiKs = light.color * Ks;
        
    float NdotL = dot(N, L);
    if (NdotL < 0)
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
//    vec3 shadow_map_color = texture(shadow_maps[0], TexCoords).rgb / 10;
//    FragColor = vec4(shadow_map_color, 1);
//    return;

    vec3 world_position = texture(gPosition, TexCoords).rgb;
    vec3 Kd = texture(gBaseColor, TexCoords).rgb;
    vec3 N = texture(gNormal, TexCoords).xyz;

    if (N.x == 0 && N.y == 0 && N.z == 0)
    {
        FragColor = vec4(Kd, 1);
        return;
    }

    vec3 IaKd = ambient_light * Kd;
    vec3 I = IaKd;

    for (int i = 0; i < point_light_count; i++)
    {
        if (dot(point_lights[i].color, point_lights[i].color) < 0.0001)
        {
            continue;
        }

        vec3 light_pos = point_lights[i].position;
        vec3 light_vec = light_pos - world_position.xyz;
        vec3 view_vec = camera_position.xyz - world_position.xyz;
        vec3 V = normalize(view_vec);
        vec3 L = normalize(light_vec);

        float distance_falloff = point_lights[i].intensity / length(light_vec);

        vec4 light_space_pos = light_vec.z > 0
            ? point_lights[i].light_view * vec4(world_position, 1)
            : point_lights[i].light_view_back * vec4(world_position, 1);

        float fragment_depth = -light_space_pos.z;

        // Calculate and set the X and Y coordinates  
        light_space_pos.xyz = normalize(light_space_pos.xyz);
        light_space_pos.xy /= 1.0 - light_space_pos.z;

        // convet to texture coordinates
        light_space_pos.xy = (light_space_pos.xy + vec2(1, 1)) / 2;

        light_space_pos.x = light_vec.z > 0
            ? light_space_pos.x/2
            : light_space_pos.x/2 + 0.5;

        float light_depth = texture(shadow_maps[i], light_space_pos.xy).r;

        if (light_depth + 0.025 < fragment_depth)
            continue;

        I += BRDF(
            L,
            V,
            Kd,
            specular,
            shininess,
            point_lights[i]) * distance_falloff;
    }

    FragColor = vec4(I, 1);
}