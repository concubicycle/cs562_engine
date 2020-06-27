#ifndef __HAMMERSLEY_HPP_
#define __HAMMERSLEY_HPP_

#include <cstdint>
#include <glbinding/gl/gl.h>
#include <renderer/shader_program.hpp>
#include <random>
#include <algorithm>

namespace renderer
{

	template<size_t N>
	struct hammersley_block
	{
		hammersley_block()
		{
			int kk;
			int pos = 0;
			float p, u;

			for (int k = 0; k < N; k++)
			{
				for (p = 0.5f, kk = k, u = 0.0f; kk; p *= 0.5f, kk >>= 1)
					if (kk & 1)
						u += p;
				float v = (k + 0.5) / N;
				hammersley[pos++] = u;
				hammersley[pos++] = v;
			}
		}
	
		void shuffle()
		{
			for (size_t i = 0; i < N-1; i+= 2)
			{
				int swap_index = distribution(generator);
				std::swap(hammersley[i], hammersley[swap_index]);
				std::swap(hammersley[i+1], hammersley[swap_index+1]);
			}
		}

		std::int32_t count{ N };
		float hammersley[2 * N];

	private:
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution{ 0, N - 2 };
	};

	template<size_t N = 40>
	class opengl_hammersley_block
	{
	public:
		opengl_hammersley_block()
		{
			using namespace gl;

			_bindpoint = _next_bindpoint++;

			glGenBuffers(1, &_id);
			glBindBufferBase(GL_UNIFORM_BUFFER, _bindpoint, _id);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(hammersley_block<N>), &_block, GL_STATIC_DRAW);
		}

		gl::GLuint bindpoint() { return _bindpoint; }

		void shuffle()
		{
			using namespace gl;

			_block.shuffle();
			glBindBufferBase(GL_UNIFORM_BUFFER, _bindpoint, _id);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(hammersley_block<N>), &_block, GL_STATIC_DRAW);
		}

	private:
		gl::GLuint _bindpoint;
		gl::GLuint _id;

		hammersley_block<N> _block;

		static std::atomic_uint _next_bindpoint;
	};

	template<size_t N>
	std::atomic_uint opengl_hammersley_block<N>::_next_bindpoint = std::atomic_uint(1);

}

#endif