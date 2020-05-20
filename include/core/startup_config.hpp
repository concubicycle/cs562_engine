#ifndef __STARTUP_CONFIG_H_
#define __STARTUP_CONFIG_H_

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace core
{
class startup_config
{   

public:
    const static constexpr float DefaultVolume = 0.5f;

    startup_config()
    {
        load();
    }

    bool fullscreen() const;
    std::uint32_t height() const;
	std::uint32_t width() const;
    float fov() const;
    bool backface_culling() const;
    bool free_mouse() const;
    std::string window_title() const;
    bool god_mode() const;
    bool auto_play() const;

    float music_volume() const;
    float sfx_volume() const;

    template <typename T>
    void set(const std::string& option, T value)
    {
        config_json[option] = value;
        save();
    }

private:
    json config_json;

    json make_default() const;
    void load();
    void save();
};
} // namespace core

#endif
