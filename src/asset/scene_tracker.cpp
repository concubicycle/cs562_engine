#include <asset/scene_tracker.hpp>


asset::scene_tracker::scene_tracker(
	std::string first_scene)
	: _next(first_scene)	
{
}

asset::scene_tracker::~scene_tracker()
{
}

bool asset::scene_tracker::has_next() const
{
	return _next.has_value();
}

std::string asset::scene_tracker::next()
{
	auto str = *_next;
	_next.reset();
	return str;
}

void asset::scene_tracker::change_scene(const std::string& next_scene)
{
	_next = next_scene;
}

void asset::scene_tracker::exit()
{
	_next.reset();
}
