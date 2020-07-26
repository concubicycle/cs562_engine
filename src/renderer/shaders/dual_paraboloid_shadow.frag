#version 430 core

#define EDGE_EPSILON 0.1

out vec4 FragColor;


in VS_OUT {
	highp float depth;
	vec2 xy;
} fs_in;

float edgeness()
{
	float edge_proximity = 1 - fs_in.xy.x*fs_in.xy.x - fs_in.xy.y*fs_in.xy.y;

	if (edge_proximity < EDGE_EPSILON) return 1;

	return 0.1 * edge_proximity + 0.975;
}

void main()
{
//	float edge_proximity = fs_in.xy.x*fs_in.xy.x + fs_in.xy.y*fs_in.xy.y; // goes to 1
//	float edgeness_factor = edge_proximity > (1-EDGE_EPSILON)
//		? 5 * edge_proximity - 4.5 
//		: 0;
//
	float depth = fs_in.depth;// + edgeness_factor * (1 - fs_in.depth);
	float sq = depth * depth;
	float cube = sq * depth;

	FragColor = vec4(
		depth,
		sq,
		cube,
		cube * depth
	);
}
