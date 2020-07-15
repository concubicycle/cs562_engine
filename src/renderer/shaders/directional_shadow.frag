#version 430 core

#define EDGE_EPSILON 0.1

out vec4 FragColor;

in VS_OUT {
	highp float depth;
	vec2 xy;
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
