#version 430 core
out vec4 FragColor;

#define PHONG_ALPHA_CUTOFF 0.0001
#define EPSILON 0.000000001
#define EDGE_EPSILON 0.5
#define SHADOW_BIAS 0.00000001
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

in vec2 TexCoords;


// ambient occlusion
uniform float ao_range_of_influence = 3.0;
uniform float ao_c_coefficient = 0.1;
uniform float ao_n = 20.0;
uniform float ao_sigma = 0.001;
uniform float ao_s = 1;
uniform float ao_k = 1;


uniform mat4 view;


//////////////////////////////////////
////////// AMBIENT OCCLUSION /////////
float ao_heaviside_step(float value)
{
    return value < 0 ?  0 : 1;
}


vec2 ao_choose_point_texcoords(int i, vec3 P, vec3 N, float d)
{
    ivec2 xy_prime = ivec2(gl_FragCoord.x, gl_FragCoord.y);
    vec2 xy = TexCoords;

    float phi = (30 * xy_prime.x ^ xy_prime.y) + 10 * xy_prime.x * xy_prime.y;
    float alpha = (i + 0.5) / ao_n;
    float h = alpha * ao_range_of_influence / d;
    float theta = TWO_PI * alpha * (7 * ao_n / 9.0) + phi;
    vec2 pi_texcoords = xy + h * vec2(cos(theta), sin(theta));
    return pi_texcoords;
}

float ao_sum_term(
    float c, 
    const in vec3 N,
    const in vec3 omega,
    float di)
{
    float h_param = ao_range_of_influence - length(omega);
    float h_eval = ao_heaviside_step(h_param);
    float numerator = max(0, dot(N, omega)-ao_sigma * di) * h_eval;
    float denominator = max(c * c, dot(omega, omega));
    return numerator / denominator;
}

float ao_sum(
    vec3 P,
    vec3 normal, 
    float d)
{
    float c = ao_c_coefficient * ao_range_of_influence;
    
    float sum = 0;

    // carefully choose a selection of n points Pi around P 
    // within specified range R. do the sum
    for (int i = 0; i < ao_n; ++i)
    {
        vec2 Pi_uv = ao_choose_point_texcoords(i, P, normal, d); 
        vec3 Pi = texture(gPosition, Pi_uv).xyz;
        vec4 Pi_normal_texel = texture(gNormal, Pi_uv);
        vec3 Pi_normal = Pi_normal_texel.xyz;
        float di = Pi_normal_texel.w;
        vec3 omega = Pi - P;
        sum += ao_sum_term(c, normal, omega, di);
    }

    sum *= TWO_PI * c / ao_n;
    return sum;
}

vec4 ao_main(vec3 P, vec3 N, float d)
{
    float sum = ao_sum(P, N, d);
    float A = pow(1 - ao_s * sum, ao_k);
    A = A < 0 ? 0 : A;
    return vec4(vec3(A), 1);
}

void main()
{
    vec4 gNormal_texel = texture(gNormal, TexCoords);
    vec4 gPosition_texel = texture(gPosition, TexCoords);

    bool not_a_surface = 
        gNormal_texel.x == 0 && 
        gNormal_texel.y == 0 && 
        gNormal_texel.z == 0 && 
        gNormal_texel.w == -1;

    if (not_a_surface) 
    {
        FragColor = vec4(1);
        return;
    }

    vec3 P = gPosition_texel.xyz;
    vec4 P_view = view * vec4(P, 1);
    vec3 N = gNormal_texel.xyz;
    float d = gNormal_texel.w;
    FragColor = ao_main(P, N, d);
}