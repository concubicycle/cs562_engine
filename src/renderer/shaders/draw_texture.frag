#version 430 core

layout(binding = 0) uniform sampler2D color;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
	FragColor = texture(color, TexCoords);
}