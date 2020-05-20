//
// Created by sava on 1/23/20.
//

#include <fstream>

#include <asset/scene.hpp>
#include <asset/resource_id.hpp>


asset::scene::scene(std::string file_path, asset_loader& assets) : _assets(assets)
{
	// read a JSON file
	auto j = assets.get_json(file_path);    

	std::vector<json> descendant_children;
	std::vector<scene_entity*> inserted_children;

    inserted_children.reserve(j["entities"].size()*512);
    descendant_children.reserve(j["entities"].size()*512);

    _entities.reserve(j["entities"].size());

    int parents_count = 0;
    int descendant_count = 0;

	for (auto entity : j["entities"])
	{
		if (entity.find("parent_id") == entity.end())
		{
			_entities.emplace_back(entity, assets);
			inserted_children.push_back(&_entities.back());
		}
		else
		{
			descendant_children.push_back(entity);
		}
	}

    while (!descendant_children.empty())
    {
        auto no_change = true;

        for (size_t i = 0; i < inserted_children.size(); ++i)   //auto* potential_parent : inserted_children)
        {
            auto potential_parent = inserted_children[i];

            if (!potential_parent->has_id()) continue;

            auto parent_id = potential_parent->id();

            for (auto& descendant : descendant_children)
            {
                auto descendant_parent_id = descendant["parent_id"].get<entity_id>();
                if (descendant_parent_id == parent_id)
                {
                    auto& inserted = potential_parent->add_child(descendant, assets);
                    inserted_children.push_back(&inserted);
                    no_change = false;
                }
            }

            auto removal = std::remove_if(descendant_children.begin(), descendant_children.end(),
                [parent_id](json& j) { return j["parent_id"].get<std::uint32_t>() == parent_id; });
            descendant_children.erase(removal, descendant_children.end());
        }

        if (no_change) break;
    }
}

asset::scene_entity& asset::scene::add_from_prototype(const std::string& file_path)
{
    auto j = _assets.get_json(file_path);
    return _entities.emplace_back(j, _assets);
}

