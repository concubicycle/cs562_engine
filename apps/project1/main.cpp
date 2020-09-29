#include "run_app.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main(int argc, char** argv) 
{
	try
	{
		run_app();
	}
	catch (const std::runtime_error& e) {
		spdlog::error("runtime_error: {}", e.what());
		return 1;
	}
	catch (const std::exception& e) {		
		spdlog::error("exception: {}", e.what());
		return 1;
	}
	catch (...) {
		 spdlog::error("Unknown Exception");
		 return 1;
	}

	return 0;
	
}
