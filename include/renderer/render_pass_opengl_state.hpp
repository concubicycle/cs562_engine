#ifndef __RENDER_PASS_OPENGL_STATE_HPP_
#define __RENDER_PASS_OPENGL_STATE_HPP_

#include <cstdint>
#include <vector>
#include <utility>
#include <limits>
#include <array>
#include <algorithm>
#include <glbinding/gl/gl.h>

namespace renderer
{
	
	// Handles changing opengl state without excessive glEnable/glDisable calls.
	class render_pass_opengl_state
	{		
		//	first:	parameter to glEnable/glDisable, like GL_BLEND
		//	second: pointer to either glEnable or glDisable		
		using cap_toggle_pair = std::pair<gl::GLenum, void((*)(gl::GLenum))>;

		static constexpr size_t MaxTrackedCaps = 16;
	
	public:

		render_pass_opengl_state(
			std::vector<cap_toggle_pair> cap_toggles_in,
			gl::GLboolean depth_mask = gl::GL_FALSE,
			gl::GLenum src_blend_factor = gl::GLenum::GL_SRC_ALPHA,
			gl::GLenum dst_blend_factor = gl::GLenum::GL_ONE_MINUS_SRC_ALPHA);

		render_pass_opengl_state(const render_pass_opengl_state& other);
		void set_opengl_state();
		void transition_from(const render_pass_opengl_state& other);

	private:				
		std::array<cap_toggle_pair, MaxTrackedCaps> cap_toggles;
		
		std::uint8_t cap_toggle_count = 0;
		gl::GLenum source_blend_factor = gl::GLenum::GL_SRC_ALPHA;
		gl::GLenum destination_blend_factor = gl::GLenum::GL_ONE_MINUS_SRC_ALPHA;

		gl::GLboolean _depth_mash;


		void fill_cap_array(std::vector<cap_toggle_pair>& caps);
	};

}


#endif