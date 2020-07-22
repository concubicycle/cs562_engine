#version 430 core

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.28318530718


out vec4 FragColor;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 4) uniform sampler2D F_table;


uniform float width;
uniform float height;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float beta = 0.05;

uniform float F_table_resolution = 1024;

in VS_OUT {
    float view_depth;
    vec4 fragment_position;
} fs_in;


float F_lookup(float u, float v)
{
    u = u / F_table_resolution;
    v = v / (PI/2.0);

    vec2 texCoords = vec2(u, v);

    return texture(F_table, texCoords).r;
}

vec3 A0(float Tsv, float gamma, vec3 color)
{
    float beta_sq = beta * beta;
    float cos_gamma = cos(gamma);
    float sin_gamma = sin(gamma);
    
    vec3 numerator = beta_sq * color  * exp(-Tsv * cos_gamma);
    float denominator = 2 * PI * Tsv * sin_gamma;
    return numerator / denominator;
}

float A1(float Tsv, float gamma)
{
    return Tsv * sin(gamma);
}

vec3 LaFinite(float Tsv, float Tvp, float gamma, vec3 color)
{
    float cos_gamma = cos(gamma);
    float sin_gamma = sin(gamma);
    float atan_term = atan(Tvp - Tsv * cos_gamma, Tsv * sin_gamma);

    float A1_eval = A1(Tsv, gamma);

    float u1 = A1_eval;
    float u2 = A1_eval;

    float v1 = PI/4 + 0.5 * atan_term;
    float v2 = gamma / 2.0;

    vec3 A0_eval  = A0(Tsv, gamma, color);

    return A0_eval * (F_lookup(u1, v1) - F_lookup(u2, v2));
}



//////////////////////////////////////
/////////// Tone Mapping ////////////
vec3 linearToSrgb(vec3 linear)
{
    linear = linear / (linear + vec3(1.0));
    return pow(linear, vec3(1.0/2.2));  
}





void main()
{
    vec2 TexCoords = gl_FragCoord.xy;
    TexCoords.xy /= vec2(width, height);
    
    vec4 gNormal_texel = texture(gNormal, TexCoords);
    vec4 gPosition_texel = texture(gPosition, TexCoords);
    
    vec3 eye_to_light = light_position - eye_position;
    vec3 eye_to_surface = fs_in.fragment_position.xyz - eye_position;
    vec3 eye_to_light_dir = normalize(eye_to_light);
    vec3 eye_to_surface_dir = normalize(eye_to_surface);

    float dprod = dot(eye_to_light_dir, eye_to_surface_dir);

    float gamma = acos(dprod);

    float Tsv = beta * length(eye_to_light);
    float Tvp = beta * fs_in.view_depth; //length(eye_to_surface);

    float colval = 10000;

    vec3 color = 
        Tvp * vec3(0.3, 0.3, 0.3);    
        //LaFinite(Tsv, Tvp, gamma, vec3(colval));

    float backwall_depth = gNormal_texel.w;

    bool not_a_surface = 
        gNormal_texel.x == 0 && 
        gNormal_texel.y == 0 && 
        gNormal_texel.z == 0 && 
        gNormal_texel.w == -1;

    if (fs_in.view_depth > (backwall_depth) && !not_a_surface)
        discard;
 
    float s0 = gl_FrontFacing ? 1 : -1;

    if (color.x < 0)
        FragColor = vec4(1, 0, 0, 1);
    else if (color.y < 0)
        FragColor = vec4(0, 1, 0, 1);
    else if (color.z < 0)
        FragColor = vec4(0, 0, 1, 1);
    else
        FragColor = vec4(s0 * color, 1);
}
