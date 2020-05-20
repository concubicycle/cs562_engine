//
// Created by sava on 1/29/20.
//

#ifndef WIZARDENGINE_AUDIO_LOADER_HPP
#define WIZARDENGINE_AUDIO_LOADER_HPP

#include <asset/component_loader.hpp>
#include <audio/audio_emitter.hpp>
#include <asset/asset_loader_node.hpp>

namespace audio
{
    class loader_emitter : public asset::component_loader
    {
    public:
        loader_emitter(util::string_table& hashes) : _hashes(hashes) {}

        void load(asset::asset_loader_node& node)
        {
            auto entity = node.entity_resource.entity;
            auto entity_data = node.entity_resource.entity_data;
            auto& json = node.entity_resource.entity_data->component_data(audio_emitter::component_bitshift);
            auto& emitter = entity->get_component<audio_emitter>();

            emitter.sound_count = 0;
            for (auto& s : json["sounds"])
            {
                auto path = s["file"].get<std::string>();
                emitter.emitter_sounds[emitter.sound_count++] = { 
                    _hashes.hash_and_store(path),
                    s.value("loop", false), 
                    s.value("volume", 1.f) 
                };
            }
        }

        component_bitset components_to_load()
        {
            return audio_emitter::archetype_bit;
        }

    private:
        util::string_table& _hashes;
    };
}

#endif //WIZARDENGINE_AUDIO_LOADER_HPP
