//
// Created by sava on 1/27/20.
//

#include <iostream>

#include <ecs/system_base.hpp>
#include <audio/audio_system.hpp>
#include <fmod_errors.h>
#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <collisions/rigid_body.hpp>
#include <audio/music_player.hpp>


bool succeededOrWarn(const std::string &message, FMOD_RESULT result);
FMOD_RESULT F_CALLBACK channelGroupCallback(FMOD_CHANNELCONTROL *channelControl,
                                            FMOD_CHANNELCONTROL_TYPE controlType,
                                            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                                            void *commandData1,
                                            void *commandData2);


audio::audio_system::audio_system(
    util::string_table &app_strings, 
    asset::asset_manager& assets,
    core::startup_config& config)
    : _app_strings(app_strings)
    , _assets(assets)
    , _startup_config(config)
{
    FMOD_RESULT result;

    // Create the main system object.
    result = FMOD::System_Create(&_system);

    if (!succeededOrWarn("FMOD: Failed to create system object", result))
        throw;

    // Initialize FMOD.
    result = _system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (!succeededOrWarn("FMOD: Failed to initialise system object", result))
        throw;

    // Create the channel group.
    result = _system->createChannelGroup("inGameSoundEffects", &_channelGroup);
    if (!succeededOrWarn("FMOD: Failed to create in-game sound_wrapper effects channel group", result))
        throw;

    _system->set3DSettings(1.f, 40.f, 1.f);
}

void audio::audio_system::update(ecs::state &state)
{
    update_emitters(state);
    sync_transform_to_listener(state);
    _system->update();
}

void audio::audio_system::cleanup(ecs::state& state)
{
    state.each<music_player>([&](music_player& mp) {
        for (size_t i = 0; i < mp.track_count; ++i)
        {
            stop_sound(mp.tracks[i]);
            mp.tracks[i].set_null();
        }
    });

    state.each<audio_emitter>([&](audio_emitter& ae) {
        for (size_t i = 0; i < ae.sound_count; ++i)
        {
            stop_sound(ae.emitter_sounds[i]);
            ae.emitter_sounds[i].set_null();
        }
    });
}

void audio::audio_system::initialize(ecs::state& state)
{
    unsigned int listener_id = 0;
    state.each<audio_listener>([&](audio_listener& listener) {
        listener.listener_id = listener_id++;
    });

    _system->set3DNumListeners(listener_id);
}

void audio::audio_system::sync_transform_to_listener(ecs::state &state)
{
    state.each_id<transforms::transform, audio_listener>(
        [this, &state] (entity_id id, transforms::transform& t, audio_listener& l)
        {
            auto& entity = state.find_entity(id);            
            auto rb_opt = entity.get_component_opt<collisions::rigid_body>();

            glm::mat3 ltw = glm::mat3(t.local_to_world);

            l.listener_position = glm::vec3(t.local_to_world[3]);
            l.listener_velocity = rb_opt ? rb_opt->velocity : glm::vec3(0.f);
            l.listener_up = ltw[1];
            l.listener_forward = ltw[2];

            FMOD_VECTOR pos = {l.listener_position.x, l.listener_position.y, l.listener_position.z};
            FMOD_VECTOR vel = {l.listener_velocity.x, l.listener_velocity.y, l.listener_velocity.z};
            FMOD_VECTOR fwd = {l.listener_forward.x, l.listener_forward.y, l.listener_forward.z};
            FMOD_VECTOR up = {l.listener_up.x, l.listener_up.y, l.listener_up.z};

            _system->set3DListenerAttributes(l.listener_id, &pos, &vel, &fwd, &up);
        });
}

void audio::audio_system::update_emitters(ecs::state &state)
{
    state.each_id<transforms::transform, audio_emitter>([&](
            entity_id id,
            transforms::transform& transform,
            audio_emitter& e)
    {
        auto position = glm::vec3(transform.local_to_world[3]);
        auto& entity = state.find_entity(id);
        auto rb_opt = entity.get_component_opt<collisions::rigid_body>();
        auto velocity = rb_opt ? rb_opt->velocity : glm::vec3(0.f);

        for (std::uint32_t i = 0; i < e.sound_count; ++i)
        {
            auto& emitter_sound = e.emitter_sounds[i];
            handle_emitter_sound(emitter_sound, position, velocity);
        }
    });

    state.each<transforms::transform, music_player>([&](
            transforms::transform& transform,
            music_player& e)
    {
        for (std::uint32_t i = 0; i < e.track_count; ++i)
        {
            auto& track = e.tracks[i];
            handle_music_sound(track);
        }
    });
}


void audio::audio_system::stop_sound(audio::emitter_sound& sound)
{
    auto& sound_channel = sound.fmod_channel;
    sound_channel->stop();
    sound.set_null();
}

void audio::audio_system::play_sound_3d(audio::emitter_sound& sound)
{
    auto mode = FMOD_3D;
    if (sound.loop) mode |= FMOD_LOOP_NORMAL;

    auto fmod_sound = get_sound(sound.path_hash, mode);
    play_sound(fmod_sound, sound, _startup_config.sfx_volume());
}

void audio::audio_system::play_sound_non3d(audio::emitter_sound& sound)
{
    auto mode = 0;
    if (sound.loop) mode |= FMOD_LOOP_NORMAL;

    auto fmod_sound = get_sound(sound.path_hash, mode);
    play_sound(fmod_sound, sound, _startup_config.music_volume());
}

void audio::audio_system::play_sound(
    FMOD::Sound *sound, 
    audio::emitter_sound& emitter_sound, 
    float volume_scale)
{
    FMOD_RESULT result;

    // Play the sound_wrapper.
    FMOD::Channel *channel = nullptr;
    result = _system->playSound(sound, nullptr, false, &channel);
    if (!succeededOrWarn("FMOD: Failed to play sound_wrapper", result))
        return;

    channel->setVolume(emitter_sound.volume * volume_scale);
    channel->set3DMinMaxDistance(emitter_sound.volume * 30.f, 100000.f);

    // Assign the channel to the group.
    result = channel->setChannelGroup(_channelGroup);
    if (!succeededOrWarn("FMOD: Failed to set channel group on", result))
        return;

    channel->setCallback(&channelGroupCallback);
    if (!succeededOrWarn("FMOD: Failed to set callback for sound_wrapper", result))
        return;
    
    emitter_sound.state = sound_state::playing;
    emitter_sound.fmod_channel = channel;
    emitter_sound.fmod_sound = sound;
}

FMOD::Sound* audio::audio_system::get_sound(size_t hash, FMOD_MODE mode)
{
    if (_sounds.find(hash) == _sounds.end())
    {
        FMOD::Sound* sound = nullptr;
        auto& track = _app_strings[hash];
        sound = _assets.get_sound(track, _system, mode);
        _sounds.insert({ hash, sound });
    }

    return _sounds.find(hash)->second;
}

void audio::audio_system::handle_emitter_sound(
        audio::emitter_sound &emitter_sound,
        glm::vec3& t_pos,
        glm::vec3& velocity)
{
    switch (emitter_sound.state)
    {
        case sound_state::stop_requested:
        {
            stop_sound(emitter_sound);
            emitter_sound.set_null();
            break;
        }
        case sound_state::stopped:
            break;
        case sound_state::playback_requested:
            play_sound_3d(emitter_sound);
        default:
        {
            if (emitter_sound.is_null()) break;
 
            check_sound_stopped(emitter_sound);

            FMOD_VECTOR pos = { t_pos.x, t_pos.y, t_pos.z };
            FMOD_VECTOR vel = { velocity.x, velocity.y, velocity.z };
            emitter_sound.fmod_channel->set3DAttributes(&pos, &vel);
            emitter_sound.fmod_channel->setVolume(emitter_sound.volume * _startup_config.sfx_volume());
        }
    }
}

void audio::audio_system::handle_music_sound(audio::emitter_sound &emitter_sound)
{
    switch (emitter_sound.state)
    {
        case sound_state::stop_requested:
        {
            stop_sound(emitter_sound);
            emitter_sound.set_null();
            break;
        }
        case sound_state::stopped:
            break;
        case sound_state::playback_requested:
            play_sound_non3d(emitter_sound);
        default:
        {
            if (emitter_sound.is_null()) break;            
            check_sound_stopped(emitter_sound);
        }
    }

    if (emitter_sound.state == sound_state::playing)
    {
        emitter_sound.fmod_channel->setVolume(emitter_sound.volume * _startup_config.music_volume());
    }
}

void audio::audio_system::check_sound_stopped(emitter_sound& emitter_sound)
{
    bool is_playing;
    auto fmod_call_success = emitter_sound.fmod_channel->isPlaying(&is_playing) == FMOD_OK;
    if (!fmod_call_success || (!is_playing && emitter_sound.state == sound_state::playing))
    {
        emitter_sound.state = sound_state::stopped;
    }
}


//////////////////////
//////////////////////
bool succeededOrWarn(const std::string &message, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cerr << message << ": " << result << " - " << FMOD_ErrorString(result) << " - " << std::endl;
        //spdlog::error("{0}: {1} {3}", message, result, FMOD_ErrorString(result));
        return false;
    }

    return true;
}

FMOD_RESULT F_CALLBACK channelGroupCallback(FMOD_CHANNELCONTROL *channelControl,
                                            FMOD_CHANNELCONTROL_TYPE controlType,
                                            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                                            void *commandData1,
                                            void *commandData2)
{
    // too noisy spdlog::info("FMod: channel control {0}, control type {1}");
    return FMOD_OK;
}

