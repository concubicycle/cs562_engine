#version 430 core


in vec2 TexCoords;
in vec3 model_space_position;

uniform sampler2D skybox;
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gBaseColor;



void main()
{
    gPosition = vec4(1/0);
    gNormal = vec4(0, 0, 0, -1);
    gBaseColor = texture(skybox, TexCoords);
}