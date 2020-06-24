#version 430 core
out vec4 FragColor;

#define EPSILON 0.0000001
#define PI 3.1415926535897932384626433832795
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8


layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gBaseColor;
layout(binding = 3) uniform sampler2D gFresnelColorRoughness;

layout(location = 3) uniform vec3 position;
layout(location = 4) uniform vec3 color;
layout(location = 5) uniform float radius;
layout(location = 6) uniform vec3 camera_position;
layout(location = 7) uniform vec2 gbuffer_dimensions;

in VS_OUT {
    vec4 world_position;
} fs_in;

//////////////////////////////////////
/// Reflectance Equation Functions ///
vec3 schlickApproximation(const vec3 F0, float NdotL_clamp)
{    
    return F0 + (vec3(1)-F0) * pow(1 - NdotL_clamp, 5);
}

float positiveCharacteristic(float d)
{
	return d > 0.0 ? 1.0 : 0.0;
}

// D term of the specular reflectance equation 
float ggxNdf(
	vec3 N, // macrosurface normal 
    vec3 M, // microsurface normal (usually the halfway vector)
	float roughness_square) // 0 - smooth, 1 - rough
{
    float n_dot_m = dot(N, M);    
    float n_dot_m_sq = n_dot_m * n_dot_m;
    float characteristic = positiveCharacteristic(n_dot_m);
    float numerator = characteristic * roughness_square;
    float denominator = 1 + n_dot_m_sq * (roughness_square - 1);
    denominator *= denominator;
    denominator *= PI;
    return numerator / denominator;
}

vec3 ggxSpecular(
    vec3 F, 
    vec3 N, 
    vec3 H,
    float NdotL_clamp, // mu_i
    float NdotV_clamp, // mu_o
    float roughness_sq)
{
    float G_and_denominator = 0.5 / (
        NdotV_clamp * sqrt(roughness_sq + NdotL_clamp * (NdotL_clamp - roughness_sq * NdotL_clamp)) +
        NdotL_clamp * sqrt(roughness_sq + NdotV_clamp * (NdotV_clamp - roughness_sq * NdotV_clamp)));    

    float D = ggxNdf(H, N, roughness_sq);    
    return F * G_and_denominator * D;
}

vec3 lambertionDiffuse(vec3 F, vec2 TexCoords)
{
    vec3 albedo = texture(gBaseColor, TexCoords).rgb;
    return (vec3(1) - F) * albedo / PI;
}

vec3 ggxReflectance(vec3 L, vec3 V, vec3 light_color, bool metalness, vec2 TexCoords)
{   vec4 gFresnelColorRoughness_texel = texture(gFresnelColorRoughness, TexCoords);
    vec3 H = normalize(L+V);
    vec3 F0 = gFresnelColorRoughness_texel.rgb;   
    float roughness = gFresnelColorRoughness_texel.a;
    float roughness_sq = roughness * roughness;

    vec3 N = normalize(texture(gNormal, TexCoords).xyz);    
    float NdotL = dot(N, L);   
    float NdotL_clamp = max(NdotL, DOT_CLAMP);
    float NdotV_clamp = max(dot(N, V), DOT_CLAMP);

    vec3 F = schlickApproximation(F0, NdotL_clamp);

    vec3 specular = ggxSpecular(F, N, H, NdotL_clamp, NdotV_clamp, roughness_sq);
    vec3 diffuse = metalness ? vec3(0) : lambertionDiffuse(F, TexCoords);

    return (specular + diffuse) * light_color * NdotL_clamp;
}

void main()
{
    vec2 gbuffer_coords = (gl_FragCoord.xy - vec2(0.5)) / gbuffer_dimensions;

    vec4 gPosition_texel = texture(gPosition, gbuffer_coords);
    vec3 world_position = gPosition_texel.rgb;
    bool metalness = gPosition_texel.a == 1 ? true : false;
    
                
    vec3 light_vec = position - world_position.xyz;    

    float radius_sq = radius*radius;
    float light_dist_sq = dot(light_vec, light_vec);

    if (light_dist_sq > radius_sq)
    {
        FragColor = vec4(0);
        return;
    }
    
    vec3 view_vec = camera_position.xyz - world_position.xyz;
    vec3 V = normalize(view_vec);
    vec3 L = normalize(light_vec);
    vec3 N = normalize(texture(gNormal, gbuffer_coords).xyz);
    

    float one_over_rsq = 1.0/radius_sq;
    float one_over_dsq = 1.0/light_dist_sq;
    float attenuation_factor = one_over_dsq - one_over_rsq;

      vec3 brdf = ggxReflectance(
            L,
            V,            
            color,
            metalness,
            gbuffer_coords);
            
    FragColor = vec4(brdf * attenuation_factor, 1);
}
