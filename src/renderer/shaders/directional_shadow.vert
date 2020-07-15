#version 430 core


// standard vertex attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 color;

out VS_OUT {    
    highp float depth;
    vec2 xy;
} vs_out;

// view matrix must point the paraboloid along the z axis
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{    
    vec4 ortho_projected = projection * view * model * vec4(position, 1);

    vs_out.xy = (ortho_projected.xy + vec2(1)) / 2;    
    vs_out.depth = ortho_projected.z / 100;

    gl_Position = vec4(
        vs_out.xy,
        vs_out.depth * 2.0 - 1.0,
        1.0);
}

