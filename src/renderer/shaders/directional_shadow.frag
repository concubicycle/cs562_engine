#version 430 core
out vec4 FragColor;

#define EPSILON 0.0000001
#define PI 3.1415926535897932384626433832795
#define DOT_CLAMP 0.00001
#define MAX_POINT_LIGHTS 8


in VS_OUT {    
    float depth;
} fs_in;



void main()
{
    FragColor = vec4(
        fs_in.depth, 
        fs_in.depth * fs_in.depth,
        fs_in.depth * fs_in.depth * fs_in.depth, 
        fs_in.depth * fs_in.depth * fs_in.depth * fs_in.depth);
}
