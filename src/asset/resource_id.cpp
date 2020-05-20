#include <asset/resource_id.hpp>

asset::resource_id asset::next_resource_id()
{
	static std::atomic<std::uint32_t> next_id;
	return next_id++;
}