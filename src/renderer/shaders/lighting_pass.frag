#version 430 core
out vec4 FragColor;

#define DEBUG

#define PHONG_ALPHA_CUTOFF 0.0001
#define EPSILON 0.000000001
#define EDGE_EPSILON 0.5
#define SHADOW_BIAS 0.0000001
#define SHADOW_BIAS_DIRECTIONAL 0.0001
#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.2831853071795864769252867665590057683943387987502
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 4

#define MAX_HAMMERSLEY_N 128

#define POINT_LIGHT_RMIN_SQ 0.01

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gBaseColor;
layout(binding = 3) uniform sampler2D gFresnelColorRoughness;

layout(binding = 4) uniform sampler2D ao_map;

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

    sampler2D shadow_map;
};

struct DirectionalLight {
    vec3 color;
    vec3 direction;
    mat4 light_view;
    mat4 light_projection;

    sampler2D shadow_map;
};

// Lights
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int point_light_count;

uniform DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform int directional_light_count;

// ambient light(s)
uniform vec3 ambient_light;// Ia
uniform bool use_ambient_light;
uniform bool draw_only_ao;

// skydome light
uniform sampler2D skydome_light;
uniform sampler2D skydome_irradiance_map;
uniform bool use_skydome_light;
uniform vec2 skydome_size;

// camera uniforms
uniform vec3 camera_position;
uniform mat4 view;

// random number sequence
uniform HammersleyBlock {
    int N;
    float hammersley[2*MAX_HAMMERSLEY_N];
} hammersley_block;




//////////////////////////////////////
/////////// MSM SHADOWS //////////////
vec3 cholesky(
    const in vec3 m1, 
    const in vec2 m2, 
    const in float m33, 
    const in vec3 z)
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

float hamburger4MSM(
    const in vec4 b, 
    float fragment_depth, 
    float alpha)
{
    vec4 b_prime = (1-alpha) * b + alpha * vec4(0.5);
    
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

float edgeness(
    const in vec2 xy)
{
	float edge_proximity = 1 - xy.x*xy.x - xy.y*xy.y;
	if (edge_proximity > EDGE_EPSILON) return 1;

    return 0.08 * edge_proximity + 0.96;
}

float shadowIntensityG(
    int light_index,
    const in vec3 light_vec, 
    const in vec3 world_position)
{
    vec4 light_space_pos = light_vec.z > 0
            ? point_lights[light_index].light_view * vec4(world_position, 1)
            : point_lights[light_index].light_view_back * vec4(world_position, 1);

    float fragment_depth = -light_space_pos.z;
    fragment_depth /= 200;

    // calculate and set the X and Y coordinates  
    light_space_pos.xyz = normalize(light_space_pos.xyz);        
    light_space_pos.xy /= 1.0 - light_space_pos.z;

    // convet to texture coordinates
    float underscale = edgeness(light_space_pos.xy) ;
    light_space_pos.xy = (underscale * light_space_pos.xy + vec2(1)) / 2;

    light_space_pos.x = light_vec.z > 0
        ? light_space_pos.x/2
        : light_space_pos.x/2 + 0.5;

    vec4 b = texture(point_lights[light_index].shadow_map, light_space_pos.xy);    
    return hamburger4MSM(b, fragment_depth, SHADOW_BIAS);
}

float shadowIntensityG_directional(
    int light_index,
    const in vec3 world_position)
{
    vec4 light_space_pos =
        directional_lights[light_index].light_view *
        vec4(world_position, 1);

    float fragment_depth = -light_space_pos.z;
    fragment_depth /= 200;

    vec4 tex_coords = directional_lights[light_index].light_projection * light_space_pos;
    tex_coords.xy = (tex_coords.xy + vec2(1)) / 2;
    vec4 b = texture(directional_lights[light_index].shadow_map, tex_coords.xy);
    return hamburger4MSM(b, fragment_depth, SHADOW_BIAS_DIRECTIONAL);
}



//////////////////////////////////////
/// Reflectance Equation Functions ///
vec3 lambertianDiffuse(vec3 F, vec3 albedo)
{    
    return (vec3(1) - F) * albedo / PI;
}

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
vec3 schlickApproximation(const in vec3 F0, float NdotL_clamp)
{    
    return F0 + (vec3(1)-F0) * pow(1 - NdotL_clamp, 5);
}

// D term of the specular reflectance equation 
float ggxNdf(
	const in vec3 N, // macrosurface normal 
    const in vec3 M, // microsurface normal (usually the halfway vector)
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

float phongNdf(
	const in vec3 N, // macrosurface normal 
    const in vec3 M, // microsurface normal (usually the halfway vector)
	float alpha) // 0 - smooth, 1 - rough
{
    float n_dot_m = dot(N, M);
    float characteristic = positiveCharacteristic(n_dot_m);
    float n_dot_m_pow_a = pow(n_dot_m, alpha);
    return 
        characteristic *
        ((alpha + 2) / TWO_PI) *
        n_dot_m_pow_a;
}


vec3 ggxSpecular(
    const in vec3 F,
    const in vec3 N,
    const in vec3 H,
    float NdotL_clamp, // mu_i
    float NdotV_clamp, // mu_o
    float roughness_sq)
{
    float G_and_denominator = 0.5 / (
        NdotV_clamp * sqrt(roughness_sq + NdotL_clamp * (NdotL_clamp - roughness_sq * NdotL_clamp)) +
        NdotL_clamp * sqrt(roughness_sq + NdotV_clamp * (NdotV_clamp - roughness_sq * NdotV_clamp)));
    
    float D = ggxNdf(N, H, roughness_sq);
    return F * G_and_denominator * D;
}

vec3 ggxReflectance(
    const in vec3 N, 
    const in vec3 L, 
    const in vec3 V, 
    const in vec3 light_color, 
    bool metalness, 
    float roughness, 
    const in vec3 F0,
    const in vec3 albedo)
{
    vec3 H = normalize(L+V);    
    float roughness_sq = roughness * roughness;

    float NdotL_clamp = max(dot(N, L), DOT_CLAMP);
    float NdotV_clamp = max(dot(N, V), DOT_CLAMP);
    float HdotL_clamp = max(dot(H, L), DOT_CLAMP);

    vec3 F = schlickApproximation(F0, HdotL_clamp);
    vec3 specular = ggxSpecular(F, N, H, NdotL_clamp, NdotV_clamp, roughness_sq);
    vec3 diffuse = metalness ? vec3(0) : lambertianDiffuse(F, albedo);

    return (specular + diffuse) * light_color * NdotL_clamp;
}




//////////////////////////////////////
//////////////// IBL ///////////////// 
vec2 directionToUv(vec3 N)
{
    float u = 0.5 - atan(N.z, N.x) / (2 * PI);
    float v = acos(N.y) / PI;
    return vec2(u, v);
}

vec3 uvToDirection(vec2 uv)
{
    return vec3(
        cos(2.0*PI*(0.5 - uv[0])) * sin(PI * uv[1]),
        cos(PI * uv[1]),
        sin(2.0*PI*(0.5 - uv[0])) * sin(PI * uv[1])
    );
}

vec3 iblSpecularMonteCarloEstimator(
    const in vec3 N, 
    const in vec3 L, 
    const in vec3 V, 
    const in vec3 F0, 
    float roughness, 
    const in vec3 light_color)
{
    float roughness_sq = roughness*roughness;
    vec3 H = normalize(L + V);
    float NdotL_clamp = max(dot(N, L), DOT_CLAMP);
    float NdotV_clamp = max(dot(N, V), DOT_CLAMP);
    float HdotL_clamp = max(dot(H, L), DOT_CLAMP);    

    vec3 F = schlickApproximation(F0, HdotL_clamp);
    float G_and_denominator = 0.5 / (
        NdotV_clamp * sqrt(roughness_sq + NdotL_clamp * (NdotL_clamp - roughness_sq * NdotL_clamp)) +
        NdotL_clamp * sqrt(roughness_sq + NdotV_clamp * (NdotV_clamp - roughness_sq * NdotV_clamp)));

    return F  * G_and_denominator * light_color * NdotL_clamp;
}

vec3 iblSpecular(
    const in vec3 N, 
    const in vec3 V, 
    const in vec3 F0, 
    float roughness)
{
    vec3 R = normalize(2 * dot(N, V) * N - V);
    vec3 A = normalize(vec3(-R.y, R.x, 0));
    vec3 B = normalize(cross(R, A));    

    float phong_roughness = 2.0 * pow(roughness, -2.0) - 2.0;

    if (phong_roughness < PHONG_ALPHA_CUTOFF)
    return vec3(0);

    vec3 sum = vec3(0);

    for (int i = 0; i < hammersley_block.N; ++i)
    {
        int block_index = i*2;
        
        vec2 xi = vec2(
            hammersley_block.hammersley[block_index], 
            hammersley_block.hammersley[block_index+1]);
       
        float xi_1_power = 1.0 / (phong_roughness + 1.0);
        vec2 cdf_uv = vec2(xi[0]*2, 0.5* acos(pow(xi[1], xi_1_power)) / PI);
        vec3 L = uvToDirection(cdf_uv);
        vec3 omega_k = normalize(L.x * A + L.y * R + L.z * B);
        vec2 uv = directionToUv(omega_k);

        vec3 H = normalize(omega_k + V);
        float D = phongNdf(N, H, phong_roughness);
        float level = 0.5 * log2(skydome_size.x * skydome_size.y / hammersley_block.N) - 0.5 * log2(D);
        level = max(level, 0);
        vec3 incoming_light_color = textureLod(skydome_light, uv, level).rgb;

        sum += 
            iblSpecularMonteCarloEstimator(
                N, 
                omega_k,
                V,
                F0, 
                roughness,
                incoming_light_color);
    }

    return sum / hammersley_block.N;
}

vec3 iblDiffuse(
    const in vec3 Kd, 
    const in vec3 N)
{
    vec2 irradiance_uv = directionToUv(N);
    vec3 irradiance = texture(skydome_irradiance_map, irradiance_uv).rgb;
    return (Kd / PI) * irradiance;
}




//////////////////////////////////////
/////////// Tone Mapping ////////////
vec3 linearToSrgb(vec3 linear)
{
    linear = linear / (linear + vec3(1.0));
    return pow(linear, vec3(1.0/2.2));  
}




//////////////////////////////////////
/////////////// MAIN ////////////////
void main()
{
    vec4 gNormal_texel = texture(gNormal, TexCoords);
    vec3 Kd = texture(gBaseColor, TexCoords).rgb;

    if (
        gNormal_texel.x == 0 && 
        gNormal_texel.y == 0 && 
        gNormal_texel.z == 0 && 
        gNormal_texel.w == -1) // not a surface
    {        
        FragColor = vec4(linearToSrgb(Kd), 1);
        return;
    }
    
    vec4 gFresnelColorRoughness_texel = texture(gFresnelColorRoughness, TexCoords);
    vec4 gPosition_texel = texture(gPosition, TexCoords);
    
    vec3 world_position = gPosition_texel.rgb;
    bool metalness = gPosition_texel.a == 1 ? true : false;
    float roughness = gFresnelColorRoughness_texel.w;
    vec3 view_vec = camera_position.xyz - world_position;
    vec3 F0 = gFresnelColorRoughness_texel.rgb;
    vec3 N = normalize(gNormal_texel.xyz);
    vec3 V = normalize(view_vec);    
    
    vec3 I = vec3(0);

    for (int i = 0; i < point_light_count; i++)
    {
        vec3 light_pos = point_lights[i].position;
        vec3 light_vec = light_pos - world_position;        
        vec3 light_color = punctualLightFalloff(point_lights[i], dot(light_vec, light_vec));
        float shadow_intensity = shadowIntensityG(i, light_vec, world_position);
        
        vec3 L = normalize(light_vec);

        I += ggxReflectance(
            N,
            L,
            V,            
            light_color,
            metalness, 
            roughness, 
            F0,
            Kd) * (1-shadow_intensity);
    }

    for (int i = 0; i < directional_light_count; ++i)
    {
        vec3 L = directional_lights[i].direction * -1;
        float shadow_intensity = shadowIntensityG_directional(i, world_position);

        I += ggxReflectance(
            N,
            L,
            V,            
            directional_lights[i].color,
            metalness, 
            roughness, 
            F0,
            Kd) * (1-shadow_intensity);
    }

    if (use_skydome_light)
    {
        vec3 a = texture(ao_map, TexCoords).rgb;

        I += iblSpecular(N, V, F0, roughness) * a;

        if (!metalness)
            I += iblDiffuse(Kd, N) * a;
    }


#ifdef DEBUG
    vec3 a = texture(ao_map, TexCoords).rgb;
    FragColor = draw_only_ao ? vec4(linearToSrgb(a), 1) : vec4(linearToSrgb(I), 1);
#else
    FragColor = vec4(linearToSrgb(I), 1);
    //FragColor = vec4(I, 1);
#endif
}
