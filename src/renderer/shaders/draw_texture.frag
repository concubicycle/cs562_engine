#version 430 core

layout(binding = 0) uniform sampler2D color;

in vec2 TexCoords;
out vec4 FragColor;

//////////////////////////////////////
/////////// Tone Mapping ////////////
vec3 linearToSrgb(vec3 linear)
{
    linear = linear / (linear + vec3(1.0));
    return pow(linear, vec3(1.0/2.2));  
}

void main()
{
	FragColor = vec4(texture(color, TexCoords).xyz, 1);
}