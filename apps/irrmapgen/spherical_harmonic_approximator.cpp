#include "spherical_harmonic_approximator.hpp"
#include <iostream>
#include <stb/stb_image_write.h>

spherical_harmonic_approximator::spherical_harmonic_approximator(
	const asset::texture_assetf& input,
	std::array<std::uint32_t, 2> output_dimensions)
	: _input(input)
	, _output_dimensions(output_dimensions)
{
	if (input.channels < 3)
	{
		throw new std::runtime_error("Textures with less than 3 channels (rgb) unsupported.");
	}

	size_t w = output_dimensions[0];
	size_t h = output_dimensions[1];
	_output = new float[w * h * 3];
}

spherical_harmonic_approximator::~spherical_harmonic_approximator()
{
	delete[](_output);
}

void spherical_harmonic_approximator::run(const std::string& filename)
{
	static float (*base_functions[9]) (Eigen::Vector3f) = {
		Y_00, Y_10, Y_11, Y_12, Y_20, Y_21, Y_22, Y_23, Y_24
	};

	std::cout << "|---------|" << std::endl;
	_coefficients[0] = light_integral(Y_00) * A0;

	std::cout << "||--------|" << std::endl;
	_coefficients[1] = light_integral(Y_10) * A1;


	std::cout << "|||-------|" << std::endl;
	_coefficients[2] = light_integral(Y_11) * A1;
	std::cout << "||||------|" << std::endl;
	_coefficients[3] = light_integral(Y_12) * A1;
	std::cout << "|||||-----|" << std::endl;
	_coefficients[4] = light_integral(Y_20) * A2;


	std::cout << "||||||----|" << std::endl;
	_coefficients[5] = light_integral(Y_21) * A2;
	std::cout << "|||||||---|" << std::endl;
	_coefficients[6] = light_integral(Y_22) * A2;
	std::cout << "||||||||--|" << std::endl;
	_coefficients[7] = light_integral(Y_23) * A2;
	std::cout << "|||||||||-|" << std::endl;
	_coefficients[8] = light_integral(Y_24) * A2;

	generate_output();
	write_output(filename);
}

Eigen::Array3f spherical_harmonic_approximator::irradiance(Eigen::Vector3f dir)
{
	Eigen::Array3f result(0, 0, 0);

	static float (*base_functions[9]) (Eigen::Vector3f) = {
		Y_00, Y_10, Y_11, Y_12, Y_20, Y_21, Y_22, Y_23, Y_24
	};

	for (size_t i = 0; i < 9; ++i)
	{
		result += base_functions[i](dir) * _coefficients[i];
	}

	return result;
}

void spherical_harmonic_approximator::generate_output()
{
	for (size_t i = 0; i < _output_dimensions[1]; ++i)
	{
		for (size_t j = 0; j < _output_dimensions[0]; ++j)
		{
			polar_coords polar = index_to_polar_coords(i, j,
				_output_dimensions[0], _output_dimensions[1]);

			Eigen::Vector3f dir = polar_to_dir(polar);
			Eigen::Array3f color = irradiance(dir);

			size_t texel_data_index =
				3 * i * _output_dimensions[0] +
				3 * j;

			_output[texel_data_index] = color[0];
			_output[texel_data_index+1] = color[1];
			_output[texel_data_index+2] = color[2];
		}
	}
}

void spherical_harmonic_approximator::write_output(const std::string& filename)
{
	size_t w = _output_dimensions[0];
	size_t h = _output_dimensions[1];
	size_t count = w * h * 3;

	// back to srgb
	for (size_t i = 0; i < count; ++i)
	{
		float linear = _output[i];
		_output[i] = std::pow(linear, 1.0 / 2.2);
	}

	stbi_write_hdr(
		filename.c_str(),
		(int)_output_dimensions[0], 
		(int)_output_dimensions[1],
		3, 
		_output);
}

float spherical_harmonic_approximator::Y_00(Eigen::Vector3f dir)
{
	static auto c = 0.5f * std::sqrt(1.f / util::Pi);
	return c;
}

float spherical_harmonic_approximator::Y_10(Eigen::Vector3f dir)
{
	static auto c = 0.5f * std::sqrt(3.f / util::Pi);
	return c * dir.y();
}

float spherical_harmonic_approximator::Y_11(Eigen::Vector3f dir)
{
	static auto c = 0.5f * std::sqrt(3.f / util::Pi);
	return c * dir.z();
}

float spherical_harmonic_approximator::Y_12(Eigen::Vector3f dir)
{
	static auto c = 0.5f * std::sqrt(3.f / util::Pi);
	return c * dir.x();
}

float spherical_harmonic_approximator::Y_20(Eigen::Vector3f dir)
{
	static auto c = 0.5f * std::sqrt(15.f / util::Pi);
	return c * dir.x() * dir.y();
}

float spherical_harmonic_approximator::Y_21(Eigen::Vector3f dir)
{
	static auto c = 0.5f * std::sqrtf(15.f / util::Pi);
	return c * dir.y() * dir.z();
}

float spherical_harmonic_approximator::Y_22(Eigen::Vector3f dir)
{
	static auto c = 0.25f * std::sqrtf(5.f / util::Pi);
	float z_sq = dir.z() * dir.z();
	return c * (3.f * z_sq - 1);
}

float spherical_harmonic_approximator::Y_23(Eigen::Vector3f dir)
{
	static auto c = -0.5f * std::sqrtf(15.f / util::Pi);
	return c * dir.x() * dir.z();
}

float spherical_harmonic_approximator::Y_24(Eigen::Vector3f dir)
{
	static auto c = 0.25f * std::sqrtf(15.f / util::Pi);
	float x_sq = dir.x() * dir.x();
	float y_sq = dir.y() * dir.y();

	return c * (x_sq - y_sq);
}

polar_coords spherical_harmonic_approximator::index_to_polar_coords(
	size_t i, 
	size_t j,
	std::uint32_t width, 
	std::uint32_t height)
{
	polar_coords coords;
	coords.theta = util::Pi_f * (i + 0.5f) / height;
	coords.phi = util::TwoPi_f * (j + 0.5f) / width;
	return coords;
}

Eigen::Vector3f spherical_harmonic_approximator::polar_to_dir(polar_coords coords)
{
	float sin_theta = std::sin(coords.theta);
	float cos_theta = std::cos(coords.theta);
	float sin_phi = std::sin(coords.phi);
	float cos_phi = std::cos(coords.phi);

	return Eigen::Vector3f(
		sin_theta * cos_phi,
		sin_theta * sin_phi,
		cos_theta
	);
}

Eigen::Array3f spherical_harmonic_approximator::light_integral(float(*basis_function)(Eigen::Vector3f))
{
	Eigen::Array3f sum(0, 0, 0);

	for (size_t i = 0; i < _input.height; ++i)
	{
		for (size_t j = 0; j < _input.width; ++j)
		{
			polar_coords polar = index_to_polar_coords(i, j, _input.width, _input.height);
			float sin_theta = std::sin(polar.theta);

			size_t texel_data_index =
				_input.channels * i * _input.width +
				_input.channels * j;

			float* data = _input.data.get();
			float* texel = data + texel_data_index;

			Eigen::Array3f color(texel[0], texel[1], texel[2]);			
			Eigen::Vector3f dir = polar_to_dir(polar);			
			float basis_eval = basis_function(dir);

			sum += 
				color * basis_eval * 
				sin_theta * _theta_step * _phi_step;
		}
	}

	return sum / (4 * util::FourPi);
}
