#include "core/startup_config.hpp"
#include "nlohmann/json.hpp"
#include <sys/stat.h>
#include <fstream>
#include <iostream>


using json = nlohmann::json;

const std::string FileName = "startup_config.json";

void core::startup_config::load()
{
    std::fstream stream(FileName);
    if (stream)
    {
        config_json.clear();
        stream >> config_json;
    }
    else
    {
        config_json = make_default();
        stream.open(FileName, std::fstream::out);
        stream << config_json;
    }
    stream.close();
}

void core::startup_config::save()
{    
    std::ofstream stream(FileName, std::ofstream::trunc);
    stream << config_json << std::endl;
    stream.close();
}

bool core::startup_config::fullscreen() const { return config_json["fullscreen"].get<bool>(); }
std::uint32_t core::startup_config::height() const { return config_json["height"].get<std::uint32_t>(); }
std::uint32_t core::startup_config::width() const { return config_json["width"].get<std::uint32_t>(); }
float core::startup_config::fov() const { return config_json["fov"]; }
bool core::startup_config::backface_culling() const { return config_json["backfaceCulling"].get<bool>(); }
bool core::startup_config::free_mouse() const { return config_json["free_mouse"].get<bool>(); }
std::string core::startup_config::window_title() const { return config_json["window_title"].get<std::string>(); }
bool core::startup_config::god_mode() const { return config_json["god_mode"].get<bool>(); }
bool core::startup_config::auto_play() const { return config_json["auto_play"].get<bool>(); }
float core::startup_config::music_volume() const { return config_json["music_volume"]; }
float core::startup_config::sfx_volume() const { return config_json["sfx_volume"]; }

json core::startup_config::make_default() const
{
    return {
        {"fullscreen", false},
        {"height", 768 },
        {"width", 1366 },
        {"fov", 45},
        {"backfaceCulling", true},
        {"free_mouse", false},
        {"window_title", "Computer Graphics"},
        {"god_mode", false},
        {"auto_play", false},
        {"music_volume", 1.f},
        {"sfx_volume", 1.f}
    };
}
