#version 430 core

#define PI 3.1415926535897932384626433832795028841971693993751
#define HALF_PI 1.5707963267948966192313216916397514420985846996876
#define TWO_PI 6.2831853071795864769252867665590057683943387987502


out vec4 FragColor;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 4) uniform sampler2D F_table;


uniform float width;
uniform float height;
uniform vec3 light_position;
uniform vec3 light_direction;
uniform vec3 eye_position;
uniform float beta = 0.04;
uniform mat4 light_view;
uniform bool use_single_scattering;
uniform float darken_bias;
uniform float light_depth_scale;
uniform vec3 initial_intensity;
uniform float F_table_range = 10;

in VS_OUT {
    float view_depth;    
    vec4 fragment_position;
} fs_in;

//////////////////////////////////////
///////////// Aitlight ///////////////
float F_lookup(float u, float v)
{
    u /= F_table_range;
    v /= (HALF_PI);    
    return texture(F_table, vec2(u, v)).r;
}

vec3 LaFinite(float Dsv, float Dvp, float gamma, const in vec3 color)
{

    float cos_gamma = cos(gamma);
    float sin_gamma = sin(gamma);
    float Tsv = beta * Dsv;
    float Tvp = beta * Dvp;
        
    vec3 A0_eval  = 
    	(beta * color * exp(-Tsv * cos_gamma)) / 
    	(TWO_PI * Dsv * sin_gamma);

    float A1_eval = Tsv * sin_gamma;

    float u1 = A1_eval;
    float u2 = A1_eval;

    float v1 = PI/4.0 + 0.5 * atan(
    	Tvp - Tsv * cos_gamma, 
    	Tsv * sin_gamma);
    	
    float v2 = gamma / 2.0;

	vec3 result = A0_eval * (F_lookup(u1, v1) - F_lookup(u2, v2));
       
#ifdef DEBUG
    if (Tsv > 10) return vec3(-1, 0, 0);
    if (u1 > F_table_range || u2 > F_table_range) return vec3(-1, 0, 0);
    if (v1 > (HALF_PI) || v2 > (HALF_PI)) return vec3(0, -1, 0);
    if (v1 < 0 || v2 < 0) return vec3(0, 0, -1);
    if (u1 < 0 || u2 < 0) return vec3(0, 0, -1);    
    if (isnan(result.x) ||isnan(result.y) ||isnan(result.z))
        return vec3(-1, 0, -1);    
#endif

     return result;
}

vec3 LaInfinite(
    float Dsv, 
    float gamma, 
    const in vec3 color)
{
    float cos_gamma = cos(gamma);
    float sin_gamma = sin(gamma);
    float Tsv = beta * Dsv;
    
    vec3 A0_eval  = 
    	(beta * color * exp(-Tsv * cos_gamma)) / 
    	(TWO_PI * Dsv * sin_gamma);

    float A1_eval = Tsv * sin_gamma;

    return A0_eval * 
    	(F_lookup(A1_eval, HALF_PI) - F_lookup(A1_eval, gamma/2));
}


//////////////////////////////////////
/////////// Tone Mapping ////////////
vec3 linearToSrgb(vec3 linear)
{
    linear = linear / (linear + vec3(1.0));
    return pow(linear, vec3(1.0/2.2));  
}



//////////////////////////////////////
/////////////// Main /////////////////
void main()
{
    vec2 TexCoords = gl_FragCoord.xy;
    TexCoords.xy /= vec2(width, height);
    
    vec4 gNormal_texel = texture(gNormal, TexCoords);
    vec4 gPosition_texel = texture(gPosition, TexCoords);

    float backwall_depth = gNormal_texel.w;
        
    vec3 eye_to_surface = fs_in.fragment_position.xyz - eye_position;    
    vec3 eye_to_surface_dir = normalize(eye_to_surface);

    vec4 eye_from_light = light_view * vec4(eye_position, 1);
    float light_depth = -eye_from_light.z;
    vec3 light_from_eye = -light_depth * light_direction;

    float dprod = dot(normalize(light_from_eye), eye_to_surface_dir);    
    float gamma = clamp(acos(dprod), 0.0, HALF_PI);

    float Dsv = light_depth * light_depth_scale;
    float Dvp = length(eye_to_surface);

    bool not_a_surface = 
        gNormal_texel.x == 0 && 
        gNormal_texel.y == 0 && 
        gNormal_texel.z == 0 && 
        gNormal_texel.w == -1;

    if (fs_in.view_depth > (backwall_depth) && !not_a_surface)
        Dvp = backwall_depth;

    vec3 color = use_single_scattering
        ? not_a_surface
            ? LaInfinite(Dsv, gamma, vec3(initial_intensity))
            : LaFinite(Dsv, Dvp, gamma, vec3(initial_intensity))
        : beta * Dvp * vec3(0.3, 0.3, 0.3);
 
    float s0 = gl_FrontFacing ? 1 : -1 - darken_bias;

#ifdef DEBUG
    if (color.x < 0)
        FragColor = vec4(1, 0, 0, 1);
    else if (color.y < 0)
        FragColor = vec4(0, 1, 0, 1);
    else if (color.z < 0)
        FragColor = vec4(0, 0, 1, 1);
    else
        FragColor = vec4(s0 * color, 1);
#else
    FragColor = vec4(s0 * color, 1);
#endif
        
}
