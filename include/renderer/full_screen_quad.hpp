#ifndef __FULL_SCREEN_QUAD_HPP_
#define __FULL_SCREEN_QUAD_HPP_

#include <glbinding/gl/gl.h>
#include <renderer/shapes.hpp>


namespace renderer
{
	class full_screen_quad
	{
		struct textured_quad_vertex
		{
			Eigen::Vector3f position;
			Eigen::Vector2f texture_coords;
		};

		textured_quad_vertex textured_quad[4] = {
			// positions        // texture Coords
			{{-1.0f,  1.0f, 0.0f},  {0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.0f},  {0.0f, 0.0}},
			{{1.0f, 1.0f, 0.0f},    {1.0f, 1.0f}},
			{{1.0f, -1.0f, 0.0f},   {1.0f, 0.0f}}
		};

	public:
		full_screen_quad()
		{
			using namespace gl;			
			
			// setup plane VAO
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(textured_quad), &textured_quad, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(textured_quad_vertex), 
				(void*)offsetof(textured_quad_vertex, position));

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 
				sizeof(textured_quad_vertex),
				(void*)offsetof(textured_quad_vertex, texture_coords));
		}

		void draw()
		{
			using namespace gl;

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

	private:
		unsigned int vao = 0;
		unsigned int vbo;
	};
}


#endif