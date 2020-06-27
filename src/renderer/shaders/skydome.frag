#version 430 core

#define PI 3.1415926535897932384626433832795


in vec2 TexCoords;
in vec3 model_space_position;

uniform sampler2D skybox;
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gBaseColor;



vec2 get_texture_coords(vec3 N)
{
    float u = 0.5 - atan(N.y, N.x) / (2 * PI);
    float v = acos(N.z) / PI;
    return vec2(u, v);
}

vec3 uvToL(vec2 uv)
{
    return vec3(
        cos(2.0*PI*(0.5 - uv[0])) * sin(PI * uv[1]),        
        sin(2.0*PI*(0.5 - uv[0])) * sin(PI * uv[1]),
        cos(PI * uv[1])
    );
}

vec2 get_texture_coords_checked(vec3 N)
{
    vec2 uv = get_texture_coords(N);
    vec3 L = uvToL(uv);
    return get_texture_coords(L);
}


void main()
{
    gPosition = vec4(1/0);
    gNormal = vec4(0, 0, 0, -1);

    gBaseColor = 
        //vec4(TexCoords, 0, 1); 
        texture(skybox, get_texture_coords_checked(model_space_position));
        //texture(skybox, TexCoords);
}