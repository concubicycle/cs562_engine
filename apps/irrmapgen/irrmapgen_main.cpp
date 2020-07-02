#include "irrmapgen.hpp"
#include <stdexcept>
#include <iostream>


int main(int argc, char** argv)
{
	try
	{
		generate_output(argc, argv);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}