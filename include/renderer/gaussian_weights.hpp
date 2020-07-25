#ifndef __GAUSSIAN_WEIGHTS_HPP_
#define __GAUSSIAN_WEIGHTS_HPP_

#include <vector>
#include <ostream>
#include <functional> 
#include <algorithm>

namespace renderer
{
	class gaussian_weights
	{
	public:
		gaussian_weights(size_t floor_half_kernel);

		void generate(size_t floor_half_kernel);

		float weight_sum() const;

		friend std::ostream& operator<<(std::ostream& output, const gaussian_weights& obj)
		{
			size_t index = 0;
			for (auto weight : obj._weights)
				output << index++ << ": " << weight << std::endl;
			return output;
		}

		const std::vector<float>& weights() const { return _weights; }
		const std::vector<float>& weights_one_side() const { return _weights_one_side; }

	private:
		std::vector<float> _weights;
		std::vector<float> _weights_one_side;
		int _w;
		float _s;

		void make_gaussian_weights(size_t floor_half_kernel);
	};
}


#endif