#include <iostream>
#include <renderer/projections.hpp>

int main(int argc, char** argv)
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