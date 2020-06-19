#version 430 core
out vec4 FragColor;

#define EPSILON 0.0000001
#define SHADOW_BIAS 0.000000000001
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

void main()
{
//    vec3 shadow_map_color = texture(shadow_maps[0], TexCoords).rgb;
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
        fragment_depth /= 100;

        // Calculate and set the X and Y coordinates  
        light_space_pos.xyz = normalize(light_space_pos.xyz);
        light_space_pos.xy /= 1.0 - light_space_pos.z;

        // convet to texture coordinates
        light_space_pos.xy = (light_space_pos.xy + vec2(1)) / 2;

        light_space_pos.x = light_vec.z > 0
            ? light_space_pos.x/2
            : light_space_pos.x/2 + 0.5;

        vec4 b = texture(shadow_maps[i], light_space_pos.xy);
        float shadow_intensity = hamburger4MSM(b, fragment_depth);

        I += BRDF(
            L,
            V,
            Kd,
            specular,
            shininess,
            point_lights[i]) * distance_falloff * (1-shadow_intensity);
    }

    FragColor = vec4(I, 1);
}