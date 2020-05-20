#ifndef __SCENE_CONTEXT_HPP_
#define __SCENE_CONTEXT_HPP_

#include <optional>
#include <string>


namespace asset
{
	class scene_tracker 
	{
	public:
		explicit scene_tracker(std::string first_scene);
		~scene_tracker();

		bool has_next() const;
		std::string next();
		void change_scene(const std::string& next_scene_name);
		void exit();

	private:
		std::optional<std::string> _next;

		void on_scene_change(const std::string& new_scene);
	};

}

#endif
