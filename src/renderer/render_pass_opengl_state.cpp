#include <renderer/render_pass_opengl_state.hpp>


renderer::render_pass_opengl_state::render_pass_opengl_state(
	std::vector<cap_toggle_pair> cap_toggles_in,
	gl::GLboolean depth_mask,
	gl::GLenum src_blend_factor,
	gl::GLenum dst_blend_factor)
	: source_blend_factor(src_blend_factor)
	, destination_blend_factor(dst_blend_factor)
	, _depth_mash(depth_mask)
{
	fill_cap_array(cap_toggles_in);
}

renderer::render_pass_opengl_state::render_pass_opengl_state(const render_pass_opengl_state& other)
: cap_toggles(other.cap_toggles)
, cap_toggle_count(other.cap_toggle_count)
, source_blend_factor(other.source_blend_factor)
, destination_blend_factor(other.destination_blend_factor)
, _depth_mash(other._depth_mash)
{
}


void renderer::render_pass_opengl_state::fill_cap_array(std::vector<cap_toggle_pair>& caps)
{
	cap_toggles.fill(std::make_pair(
		gl::GLenum::GL_DEBUG_OUTPUT, 
		(void((*)(gl::GLenum)))nullptr));

	for (size_t i = 0; i < caps.size() && i < MaxTrackedCaps; ++i)
	{
		cap_toggles[i] = caps[i];

		// side effect
		if (cap_toggles[i].first == gl::GLenum::GL_BLEND && cap_toggles[i].second == gl::glEnable)
		{
			gl::glBlendFunc(source_blend_factor, destination_blend_factor);
		}
	}

	auto sort_func = [](const cap_toggle_pair& a, const cap_toggle_pair& b) {
		return a.first < b.first;
	};

	std::sort(cap_toggles.begin(), cap_toggles.end(), sort_func);
	cap_toggle_count = caps.size() < MaxTrackedCaps 
		? (std::uint8_t)caps.size() 
		: (std::uint8_t)MaxTrackedCaps;
}


void renderer::render_pass_opengl_state::transition_from(const renderer::render_pass_opengl_state& other)
{
	size_t this_cap_setter_index = 0;
	size_t other_cap_setter_index = 0;

	while (this_cap_setter_index < cap_toggle_count && other_cap_setter_index < other.cap_toggle_count)
	{
		auto& p_this = cap_toggles[this_cap_setter_index];
		auto& p_other = other.cap_toggles[other_cap_setter_index];

		if (p_this.first == p_other.first) // same cap
		{
			if (p_this.second != p_other.second)
				p_this.second(p_this.first);

			this_cap_setter_index++;
			other_cap_setter_index++;
		}
		else if (p_this.first < p_other.first)
		{
			p_this.second(p_this.first);
			this_cap_setter_index++;
		}
		else
		{
			other_cap_setter_index++;
		}
	}

	while (this_cap_setter_index < cap_toggle_count)
	{
		auto& p_this = cap_toggles[this_cap_setter_index++];
		p_this.second(p_this.first);
	}

	if (_depth_mash != other._depth_mash)
		gl::glDepthMask(_depth_mash);
}


void renderer::render_pass_opengl_state::set_opengl_state()
{
	for (size_t i = 0; i < cap_toggle_count; ++i)
	{
		auto& cap = cap_toggles[i];
		cap.second(cap.first);
		if (cap.first == gl::GLenum::GL_BLEND)
			gl::glBlendFunc(source_blend_factor, destination_blend_factor);
	}

	gl::glDepthMask(_depth_mash);
}