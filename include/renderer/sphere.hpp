#ifndef __REND_SPHERE_HPP_
#define __REND_SPHERE_HPP_

#include <glbinding/gl/gl.h>
#include <cstdint>
#include <Eigen/Core>
#include <vector>
#include <util/constants.hpp>

namespace renderer
{
	class sphere
	{
	public:

		sphere(std::uint32_t stacks, std::uint32_t sectors)
		{
			using namespace gl;

			make_mesh(stacks, sectors);

			// setup plane VAO
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Eigen::Vector3f), (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * _indices.size(), _indices.data(), GL_STATIC_DRAW);

			glBindVertexArray(0);
		}

		unsigned int get_vao() const {
			return vao;
		}

		unsigned int get_ebo() const {
			return ebo;
		}

		size_t get_index_count() const { return _indices.size(); }
		
	private:
		std::vector<Eigen::Vector3f> _vertices;
		std::vector<std::uint32_t> _indices;
		unsigned int vao = 0;
		unsigned int vbo;
		unsigned int ebo;

		void make_mesh(std::uint32_t stacks, std::uint32_t sectors)
		{
			auto stack_inc = util::Pi / stacks;
			auto sector_inc = util::TwoPi / sectors;

			for (std::uint32_t stack = 0; stack <= stacks; ++stack)
			{
				// -pi/2 to pi/2
				auto phi = -util::HalfPi + stack * stack_inc;
				auto cos_phi = std::cos(phi);
				auto sin_phi = std::sin(phi);

				for (std::uint32_t sector = 0; sector < sectors; ++sector)
				{
					auto theta = sector * sector_inc;
					auto cos_theta = std::cos(theta);
					auto sin_theta = std::sin(theta);

					float x = cos_phi * cos_theta;
					float y = cos_phi * sin_theta;
					float z = sin_phi;
					_vertices.emplace_back(x, y, z);
				}
			}

			for (std::uint32_t stack = 0; stack < stacks; ++stack)
			{
				auto ring_start = stack * sectors;
				auto next_ring_start = (stack + 1) * sectors;

				for (std::uint32_t sector = 0; sector < sectors; ++sector)
				{
					auto next_sector = (sector + 1) % sectors;

					auto lower_left = ring_start + sector;
					auto upper_left = next_ring_start + sector;
					auto lower_right = ring_start + next_sector;
					auto upper_right = next_ring_start + next_sector;

					_indices.push_back(lower_left);
					_indices.push_back(upper_right);
					_indices.push_back(upper_left);

					_indices.push_back(lower_left);
					_indices.push_back(lower_right);
					_indices.push_back(upper_right);
				}
			}
		}
	};
}


#endif