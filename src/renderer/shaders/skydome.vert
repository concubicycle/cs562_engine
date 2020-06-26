#version 430 core

#define PI 3.1415926535897932384626433832795


layout (location = 0) in vec3 position;

out vec2 TexCoords;
out vec3 model_space_position;

uniform mat4 projection;
uniform mat4 view;

vec2 get_texture_coords(vec3 N)
{
    float u = 0.5 - atan(N.z, N.x) / (2 * PI);
    float v = acos(N.y) / PI;
    return vec2(u, v);
}

void main()
{
    TexCoords = get_texture_coords(position);
    model_space_position = normalize(position);
    gl_Position = projection * view * vec4(position, 1.0);
}