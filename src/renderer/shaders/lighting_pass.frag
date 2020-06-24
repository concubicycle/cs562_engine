#version 430 core
out vec4 FragColor;

#define EPSILON 0.000000001
#define EDGE_EPSILON 0.5
#define SHADOW_BIAS 0.00000001
#define PI 3.1415926535897932384626433832795
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8

#define POINT_LIGHT_RMIN_SQ 0.01

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gBaseColor;
layout(binding = 3) uniform sampler2D gFresnelColorRoughness;

in vec2 TexCoords;

//////////////////////////////////////
////////////// UNIFORMS //////////////

// punctual lights
struct PointLight {
    vec3 color;
    vec3 position;
    float reference_distance;
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




//////////////////////////////////////
/////////// MSM SHADOWS //////////////
vec3 cholesky(vec3 m1, vec2 m2, float m33, vec3 z)
{
    float a = sqrt(m1[0]);
    float b = m1[1] / a;
    float c = m1[2] / a;
    float d = sqrt(m2[0] - b*b);
    float e = (m2[1] - b*c) / d;
    float f = sqrt(m33 - c*c - e*e);
    
    float c_hat0 = z[0] / a;
    float c_hat1 = (z[1] - b * c_hat0) / d;
    float c_hat2 = (z[2] - c * c_hat0 - e * c_hat1) / f;

    float c2 = c_hat2 / f;
    float c1 = (c_hat1 - e*c2) / d;
    float c0 = (c_hat0 - b*c1 - c * c2) / a;

    return vec3(c0, c1, c2);
}

vec2 solve_quadratic(float a, float b, float c)
{
    float root = sqrt(b*b-4*a*c);
    float two_a = 2 * a;
    return vec2(
        (-b + root) / two_a,
        (-b - root) / two_a
    );
}

float hamburger4MSM(vec4 b, float fragment_depth)
{
    vec4 b_prime = (1-SHADOW_BIAS) * b + SHADOW_BIAS * vec4(0.5);
    
    vec3 m1 = vec3(1, b_prime[0], b_prime[1]);
    vec2 m2 = vec2(b_prime[1], b_prime[2]);
    float m33 = b_prime[3];
    vec3 z = vec3(1, fragment_depth, fragment_depth*fragment_depth);

    vec3 c = cholesky(m1, m2, m33, z);

    vec2 z_roots = solve_quadratic(c[2], c[1], c[0]);
    
    float zf = fragment_depth;
    float z2 = min(z_roots[0], z_roots[1]);
    float z3 = max(z_roots[0], z_roots[1]);

    if (fragment_depth <= z2) return 0;
    else if (fragment_depth <= z3)
    {
        float numerator = zf * z3 - b_prime[0] * (zf + z3) + b_prime[1];
        float denominator = (z3 - z2) * (zf - z2);
        return numerator / denominator;
    }

    float numerator = z2 * z3 - b_prime[0] * (z2 + z3) + b_prime[1];
    float denominator = (zf - z2) * (zf - z3);
        
    return 1 - numerator / denominator;
}

float edgeness(vec2 xy)
{
	float edge_proximity = 1 - xy.x*xy.x - xy.y*xy.y;
	if (edge_proximity > EDGE_EPSILON) return 1;

    return 0.2 * edge_proximity + 0.9;
}

float shadowIntensityG(
    int light_index,
    const vec3 light_vec, 
    const vec3 world_position)
{
    vec4 light_space_pos = light_vec.z > 0
            ? point_lights[light_index].light_view * vec4(world_position, 1)
            : point_lights[light_index].light_view_back * vec4(world_position, 1);

        float fragment_depth = -light_space_pos.z;
        fragment_depth /= 100;

        // Calculate and set the X and Y coordinates  
        light_space_pos.xyz = normalize(light_space_pos.xyz);        
        light_space_pos.xy /= 1.0 - light_space_pos.z;

        // convet to texture coordinates
        float underscale = edgeness(light_space_pos.xy) ;
        light_space_pos.xy = (underscale * light_space_pos.xy + vec2(1)) / 2;

        light_space_pos.x = light_vec.z > 0
            ? light_space_pos.x/2
            : light_space_pos.x/2 + 0.5;

        vec4 b = texture(shadow_maps[light_index], light_space_pos.xy);
        return hamburger4MSM(b, fragment_depth);
}




//////////////////////////////////////
/// Reflectance Equation Functions ///
vec3 punctualLightFalloff(const PointLight light, float r_square)
{
    r_square = r_square < 0 ? 0 : r_square;
    float r0_sq = light.reference_distance * light.reference_distance;
    return light.color * r0_sq * 1 / max(POINT_LIGHT_RMIN_SQ, r_square);
}

float positiveCharacteristic(float d)
{
	return d > 0.0 ? 1.0 : 0.0;
}

// F term
vec3 schlickApproximation(const vec3 F0, float NdotL_clamp)
{    
    return F0 + (vec3(1)-F0) * pow(1 - NdotL_clamp, 5);
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

vec3 lambertianDiffuse(vec3 F)
{
    vec3 albedo = texture(gBaseColor, TexCoords).rgb;
    return (vec3(1) - F) * albedo / PI;
}

vec3 ggxBRDF(vec3 L, vec3 V, vec3 light_color, bool metalness)
{   
    vec4 gFresnelColorRoughness_texel = texture(gFresnelColorRoughness, TexCoords);
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
    vec3 diffuse = metalness ? vec3(0) : lambertianDiffuse(F);

    return (specular + diffuse) * light_color * NdotL_clamp;
}





void main()
{
    vec4 gPosition_texel = texture(gPosition, TexCoords);

    vec3 world_position = gPosition_texel.rgb;
    bool metalness = gPosition_texel.a == 1 ? true : false;
    vec3 Kd = texture(gBaseColor, TexCoords).rgb;
    vec4 N = texture(gNormal, TexCoords);

    if (N.x == 0 && N.y == 0 && N.z == 0 && N.w == -1) // sky or something
    {
        FragColor = vec4(Kd, 1);
        return;
    }
        
    vec3 I = vec3(0);

    for (int i = 0; i < point_light_count; i++)
    {
        vec3 light_pos = point_lights[i].position;
        vec3 light_vec = light_pos - world_position;
        vec3 view_vec = camera_position.xyz - world_position;
        vec3 V = normalize(view_vec);
        vec3 L = normalize(light_vec);

        vec3 light_color = punctualLightFalloff(point_lights[i], dot(light_vec, light_vec));
        float shadow_intensity = shadowIntensityG(i, light_vec, world_position);

        I += ggxBRDF(
            L,
            V,            
            light_color,
            metalness) * (1-shadow_intensity);
    }

    FragColor = vec4(I, 1);
}
