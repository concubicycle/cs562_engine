#version 430 core
out vec4 FragColor;


layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gBaseColor;

in vec2 TexCoords;

void main()
{
    //FragColor = vec4(0, 0, 1, 1);
    FragColor = vec4(texture(gNormal, TexCoords).rgb, 1);
}