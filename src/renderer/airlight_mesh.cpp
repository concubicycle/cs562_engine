#include <renderer/airlight_mesh.hpp>

renderer::airlight_mesh::airlight_mesh(
	std::uint32_t shadowmap_width, 
	std::uint32_t shadowmap_height)
{
	using namespace gl;

	build_mesh(shadowmap_width, shadowmap_height);
		
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);	
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_size_bytes(), _vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size_bytes(), _indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Eigen::Vector3f), 0);

	glBindVertexArray(0);
}

void renderer::airlight_mesh::build_mesh(
	std::uint32_t shadowmap_width,
	std::uint32_t shadowmap_height)
{
	float width_inc = 1.f / shadowmap_width;
	float height_inc = 1.f / shadowmap_height;

	// first row
	for (size_t j = 0; j < shadowmap_width; ++j)
	{
		_vertices.emplace_back(j * width_inc, 0, 0);
	}

	// last shadowmap_height-1 rows
	for (size_t i = 1; i < shadowmap_height; ++i)
	{
		// create row
		for (size_t j = 0; j < shadowmap_width; ++j)
		{
			_vertices.emplace_back(j * width_inc, i * height_inc, 0);
		}

		// link this and previous rows
		size_t this_index_start = i * shadowmap_width;
		size_t last_index_start = (i - 1) * shadowmap_width;

		for (size_t j = 0; j < shadowmap_width - 1; ++j)
		{
			_indices.push_back(this_index_start + j); 
			_indices.push_back(last_index_start + j + 1);
			_indices.push_back(this_index_start + j + 1);
			
			_indices.push_back(this_index_start + j); 
			_indices.push_back(last_index_start + j);
			_indices.push_back(last_index_start + j + 1);
		}
	}	

	// add ligh-position vertex
	_vertices.emplace_back(0.5, 0.5, -0.999);
	
	auto lightvertex = _vertices.size() - 1;
	auto lowerleft = 0;
	auto lowerright = shadowmap_width - 1;
	auto upperleft = (shadowmap_height - 1) * shadowmap_width;
	auto upperright = upperleft + (shadowmap_width - 1);


	// w=4, h=3
	// 8  9  10 11
	// 4  5  6  7
	// 0  1  2  3

	// link light vertex to bottom row
	for (size_t j = lowerleft; j < lowerright; ++j)
	{
		_indices.push_back(lightvertex);
		_indices.push_back(j + 1);
		_indices.push_back(j);
	}

	// link light vertex to top row
	for (size_t j = upperleft; j < upperright; ++j)
	{
		_indices.push_back(lightvertex);
		_indices.push_back(j);
		_indices.push_back(j + 1);
	}

	// link light vertex to left col
	for (size_t i = lowerleft; i < upperleft; i += shadowmap_width)
	{
		_indices.push_back(lightvertex);
		_indices.push_back(i);
		_indices.push_back(i + shadowmap_width);
	}

	// link light vertex to left col
	for (size_t i = lowerright; i < upperright; i += shadowmap_width)
	{
		_indices.push_back(lightvertex);
		_indices.push_back(i + shadowmap_width);
		_indices.push_back(i);
	}
}

void renderer::airlight_mesh::draw()
{
	using namespace gl;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

size_t renderer::airlight_mesh::vertices_size_bytes() const
{
	return sizeof(Eigen::Vector3f) * _vertices.size();
}

size_t renderer::airlight_mesh::indices_size_bytes() const
{
	return _indices.size() * sizeof(std::uint32_t);
}