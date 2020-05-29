#version 430 core
out vec4 FragColor;

#define EPSILON 0.0000001
#define PI 3.1415926535897932384626433832795
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8


layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gBaseColor;

uniform vec3 position;
uniform vec3 color;
uniform float radius;
uniform vec3 camera_position;
uniform vec2 gbuffer_dimensions;

in VS_OUT {
    vec4 world_position;
} fs_in;

vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 Kd, vec3 Ks, float alpha)
{    
    vec3 H = normalize(L+V);    

    vec3 IaKd = vec3(0);
    vec3 IiKd = color * Kd;
    vec3 IiKs = color * Ks;
        
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

    return color * NdotL * brdf;
}


void main()
{
    vec2 gbuffer_coords = (gl_FragCoord.xy - vec2(0.5)) / gbuffer_dimensions;

    vec3 world_position = texture(gPosition, gbuffer_coords).rgb;
                
    vec3 light_vec = position - world_position.xyz;
    vec3 view_vec = camera_position.xyz - world_position.xyz;
    vec3 V = normalize(view_vec);
    vec3 L = normalize(light_vec);
    vec3 N = normalize(texture(gNormal, gbuffer_coords).xyz);
    float light_distance = length(light_vec);

    float one_over_rsq = 1.0/(radius*radius);
    float one_over_dsq = 1.0/(light_distance*light_distance);
    float attenuation_factor = one_over_dsq - one_over_rsq;

    vec3 brdf = BRDF(
        L,
        V,
        N,
        vec3(1),
        color,
        0.5);
         
    FragColor = vec4(brdf * attenuation_factor, 1);
}