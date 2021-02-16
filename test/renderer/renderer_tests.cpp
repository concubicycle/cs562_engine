#include <iostream>
#include <renderer/projections.hpp>
#include <renderer/gaussian_weights.hpp>
#include <renderer/render_pass_opengl_state.hpp>




void gaussian_weights_test();
void perspectives_test();
void test_ogl_state_transition();



int main(int argc, char** argv)
{
	gaussian_weights_test();
	perspectives_test();
	test_ogl_state_transition();
}


void gaussian_weights_test()
{
	renderer::gaussian_weights weights(7);
	std::cout << weights << "------" << std::endl << weights.weight_sum() << std::endl;
	const auto& half = weights.weights_one_side();

	for (auto w : half)
		std::cout << w << std::endl;
}


void perspectives_test()
{
	float fov = 0.785000026f;
	float aspect = 1.77777779;
	float near = 0.100000001f;
	float far = 4096.00000f;

	Eigen::Matrix4f perspective = renderer::perspective(fov, aspect, near, far);
	std::cout << perspective << std::endl;
	auto data = perspective.data();

	std::cout << std::endl;
	for (size_t i = 0; i < 16; ++i)
	{
		std::cout << data[i] << " ";
	}
	std::cout << std::endl;
}


void fake_glEnable(gl::GLenum val)
{
	std::cout << "Enabling " << (unsigned int) val << std::endl;
}

void fake_glDisable(gl::GLenum val)
{
	std::cout << "Disabling " << (unsigned int)val << std::endl;
}

void test_ogl_state_transition()
{
//GL_DEPTH_TEST = 0x0B71, // decimal value: 2929
//GL_STENCIL_TEST = 0x0B90, // decimal value: 2960
//GL_NORMALIZE = 0x0BA1, // decimal value: 2977
//GL_ALPHA_TEST = 0x0BC0, // decimal value: 3008
//GL_DITHER = 0x0BD0, // decimal value: 3024
//GL_BLEND = 0x0BE2, // decimal value: 3042
//GL_INDEX_LOGIC_OP = 0x0BF1, // decimal value: 3057



	renderer::render_pass_opengl_state a({
		std::make_pair(gl::GLenum::GL_DEPTH_TEST, fake_glEnable),
		std::make_pair(gl::GLenum::GL_ALPHA_TEST, fake_glEnable),
		std::make_pair(gl::GLenum::GL_BLEND, fake_glDisable)
	});

	renderer::render_pass_opengl_state b({
		std::make_pair(gl::GLenum::GL_STENCIL_TEST, fake_glDisable),
		std::make_pair(gl::GLenum::GL_ALPHA_TEST, fake_glEnable),
		std::make_pair(gl::GLenum::GL_INDEX_LOGIC_OP, fake_glDisable),
		std::make_pair(gl::GLenum::GL_DEPTH_TEST, fake_glDisable)
	});

	a.set_opengl_state();
	b.transition_from(a);

}