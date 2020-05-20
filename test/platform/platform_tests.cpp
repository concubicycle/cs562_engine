#include <platform/sleeper.hpp>
#include <stdexcept>
#include <util/running_average.hpp>
#include <iostream>

void find_avg_resolution(os::sleeper& sleeper);

int main()
{
	os::sleeper sleeper;
	
	auto epsilon = std::chrono::milliseconds(3);
	auto wait_period = std::chrono::milliseconds(1000);

	auto before= std::chrono::system_clock::now();
	sleeper.sleep(wait_period);
	auto after = std::chrono::system_clock::now();

	if (std::chrono::abs((after - before) - wait_period) > epsilon)
	{
		std::cerr << "Wait time is " << (after - before).count() << "ms" << std::endl;
		throw std::runtime_error("wait not waiting the right time");
	}
	else
	{
		std::cerr << "Latency under 3ms after one test..." << std::endl;
	}

	find_avg_resolution(sleeper);
	return 0;
}


void find_avg_resolution(os::sleeper& sleeper)
{
	std::cout << "Testing wait error" << std::endl;

	int test_iterations = 50;
	float sum = 0;
	auto wait_period = std::chrono::milliseconds(200);

	for (int i = 0; i < test_iterations; ++i)
	{
		auto before = std::chrono::high_resolution_clock::now();
		sleeper.sleep(wait_period);
		auto after = std::chrono::high_resolution_clock::now();

		auto error = std::chrono::abs((after - before) - wait_period);		
		sum += error.count();
	}

	auto avg = sum / test_iterations;
	std::cout << "Average wait error is: " << avg / 1000000000 << "s" << std::endl;
}