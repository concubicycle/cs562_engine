#version 430 core

#define EDGE_EPSILON 0.01


// http://cdn.imgtec.com/sdk-documentation/Dual+Paraboloid+Environment+Mapping.Whitepaper.pdf

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
uniform mat4 view;
uniform mat4 model;

void main()
{
    // need to return NDC, where xy are from the paraboloid 
    // normal N, and z order is preserved.
    vec4 incident = view * model * vec4(position, 1);
    vec3 incident_direction = normalize(incident.xyz);

    vs_out.depth = -incident.z;
    vec3 paraboloid_direction = vec3(0, 0, -1);
    vec3 normal = paraboloid_direction + incident_direction;
    
    // find the xy coords underneath the paraboloid intersection, 
    // which is where the pixel would go on the texture. 
    vs_out.xy = normal.xy / abs(normal.z);

    vs_out.depth /= 100;

    gl_Position = vec4(
        vs_out.xy,
        vs_out.depth * 2.0 - 1.0,
        1.0);
}

