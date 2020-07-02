#ifndef __SHERICAL_HARMONIC_APPROXIMATOR_HPP_
#define __SHERICAL_HARMONIC_APPROXIMATOR_HPP_

#include <cstdint>
#include <utility>
#include <Eigen/Core>
#include <asset/texture_asset.hpp>
#include <util/constants.hpp>
#include <array>
#include <string>

struct polar_coords
{
	float theta, phi;
};

class spherical_harmonic_approximator
{
	const float A0{ util::Pi };
	const float A1{ (2.f / 3.f) * util::Pi };
	const float A2{ 0.25f * util::Pi };
	
public:
	spherical_harmonic_approximator(
		const asset::texture_assetf& _input, 
		std::array<std::uint32_t, 2> output_dimensions);

	~spherical_harmonic_approximator();

	spherical_harmonic_approximator(const spherical_harmonic_approximator& other) = delete;
	spherical_harmonic_approximator& operator=(const spherical_harmonic_approximator& other) = delete;
	spherical_harmonic_approximator& operator=(spherical_harmonic_approximator&& other) = delete;

	void run(const std::string& output_filename);
	Eigen::Array3f irradiance(Eigen::Vector3f dir);
	void generate_output();
	void write_output(const std::string& filename);

private:
	const asset::texture_assetf& _input;
	std::array<std::uint32_t, 2> _output_dimensions;

	Eigen::Array3f _coefficients[9];

	float* _output;

	float _theta_step{ util::Pi_f / _input.height };
	float _phi_step{ util::TwoPi_f / _input.width };

	polar_coords index_to_polar_coords(
		size_t i, 
		size_t j,
		std::uint32_t width,
		std::uint32_t height);


	Eigen::Vector3f polar_to_dir(polar_coords coords);
	Eigen::Array3f light_integral(float(*basis_function)(Eigen::Vector3f));

	static float Y_00(Eigen::Vector3f dir);

	static float Y_10(Eigen::Vector3f dir);
	static float Y_11(Eigen::Vector3f dir);
	static float Y_12(Eigen::Vector3f dir);

	static float Y_20(Eigen::Vector3f dir);
	static float Y_21(Eigen::Vector3f dir);
	static float Y_22(Eigen::Vector3f dir);
	static float Y_23(Eigen::Vector3f dir);
	static float Y_24(Eigen::Vector3f dir);


};


#endif