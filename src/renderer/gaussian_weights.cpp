#include <renderer/gaussian_weights.hpp>
#include <cmath>
#include <numeric>




renderer::gaussian_weights::gaussian_weights(size_t floor_half_kernel)
{
	make_gaussian_weights(floor_half_kernel);
}

void renderer::gaussian_weights::generate(size_t floor_half_kernel)
{
	make_gaussian_weights(floor_half_kernel);
}

float renderer::gaussian_weights::weight_sum() const
{
	return std::accumulate(_weights.begin(), _weights.end(), 0.f);
}


void renderer::gaussian_weights::make_gaussian_weights(
	size_t half_kernel_floor)
{
	_w = half_kernel_floor;
	_s = (float)_w / 2.f;

	_weights.clear();
	_weights_one_side.clear();
		
	float s_sq = _s * _s;

	float sum = 0;
	for (int i = -_w; i <= _w; ++i)
	{
		float i_sq = i * i;
		float term = std::exp(-i_sq / (2.f * s_sq));
		sum += term;
		_weights.push_back(term);
	}

	float beta = 1 / sum;

	auto mult_by_beta = [beta](float f) { return f * beta; };
	std::transform(_weights.begin(), _weights.end(), _weights.begin(), mult_by_beta);

	for (int i = 0; i <= _w; ++i)
	{
		size_t index = _w + i;
		_weights_one_side.push_back(_weights[index]);
	}
}