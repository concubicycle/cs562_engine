#version 430 core
out vec4 FragColor;


in VS_OUT {
	highp float depth;
} fs_in;

void main()
{
	float depth = fs_in.depth;

	FragColor = vec4(
		depth,
		depth * depth,
		depth * depth * depth,
		depth * depth * depth * depth
	);
} 