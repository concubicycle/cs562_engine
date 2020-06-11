#version 430 core
out vec4 FragColor;


in VS_OUT {
	highp float depth;
} fs_in;

void main()
{
	FragColor = vec4(fs_in.depth);
}