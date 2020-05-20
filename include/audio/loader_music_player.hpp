#ifndef __LOADER_MUSIC_PLATER_HPP_
#define __LOADER_MUSIC_PLATER_HPP_


#include <asset/component_loader.hpp>
#include <audio/music_player.hpp>
#include <asset/asset_loader_node.hpp>
#include <util/string_table.hpp>

namespace audio
{
    class loader_music_player : public asset::component_loader
    {
    public:
        loader_music_player(util::string_table& hashes) : _hashes(hashes) {}

        void load(asset::asset_loader_node& node)
        {
            auto entity = node.entity_resource.entity;
            auto entity_data = node.entity_resource.entity_data;
            auto& json = node.entity_resource.entity_data->component_data(music_player::component_bitshift);
            auto& emitter = entity->get_component<music_player>();

            emitter.track_count = 0;
            for (auto& s : json["tracks"])
            {
                auto path = s["file"].get<std::string>();
                auto loop = s.find("loop") == s.end() ? false : s["loop"].get<bool>();
                auto volume = s.find("volume") == s.end() ? false : s["volume"].get<float>();
                auto hash = _hashes.hash_and_store(path);
                emitter.tracks[emitter.track_count++] = { hash, loop, volume };
            }
        }

        component_bitset components_to_load()
        {
            return music_player::archetype_bit;
        }

    private:
        util::string_table& _hashes;
    };
}


#endif