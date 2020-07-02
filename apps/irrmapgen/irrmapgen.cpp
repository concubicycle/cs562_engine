#include "irrmapgen.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include "spherical_harmonic_approximator.hpp"

#include <asset/asset_loader.hpp>


void print_help()
{
	std::cout << std::endl << "--- Usage ---" << std::endl;

	std::cout << "See image dimensions:" << std::endl;
	std::cout << "\tirrmapgen infile.hdr" << std::endl;

	std::cout << "Convert without scaling:" << std::endl;
	std:: cout << "\tirrmapgen infile.hdr outfile.hdr" << std::endl;

	std::cout << "Scale down by 2^power - additional parameter for \"power\":" << std::endl
		<< "\tirrmapgen infile.hdr outfile.hdr 2" << std::endl
		<< "\twill result in image of dimensions original/(2^2)" << std::endl;

	std::cout << "---" << std::endl;
}

void generate_output(int argc, char** argv)
{
	asset::asset_loader loader;

	switch (argc)
	{
	case 2:
	{
		std::string infile(argv[1]);
		auto& tex = loader.get_texturef_untonemapped(infile);
		std::cout << "Dimensions: (" << tex.width << ", " << tex.height << ")" << std::endl;
		std::cout << "Channels: " << tex.channels << std::endl;
		break;
	}
	case 3:
	{
		std::string infile(argv[1]);
		std::string outfile(argv[2]);
		auto& tex = loader.get_texturef_untonemapped(infile);
		spherical_harmonic_approximator appx(tex, {
			(std::uint32_t)tex.width,
			(std::uint32_t)tex.height });

		appx.run(outfile);

		break;
	}
	case 4:
	{
		std::string infile(argv[1]);
		std::string outfile(argv[2]);
		unsigned long power = std::stoul(argv[3], nullptr, 0);
		std::uint32_t scale_factor = std::pow(2, power);
		auto& tex = loader.get_texturef_untonemapped(infile);

		std::uint32_t new_width = (std::uint32_t)tex.width / scale_factor;
		std::uint32_t new_height = (std::uint32_t)tex.height / scale_factor;
		spherical_harmonic_approximator appx(tex, {new_width, new_height });

		appx.run(outfile);

		break;
	}
	default:
		print_help();		
		
	}
}
