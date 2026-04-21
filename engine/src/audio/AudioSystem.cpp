// Copyright (c) 2026 NoxFly — AGPL-3.0

#include <NoxEngine/audio/AudioSystem.hpp>
#include <NoxEngine/core/Logger.hpp>

#ifdef NOX_HAS_SDL3_MIXER
#include <SDL3_mixer/SDL_mixer.h>
#endif

#include <algorithm>
#include <cmath>

namespace Nox {

    AudioSystem::AudioSystem() {
        tracks_.fill(nullptr);
    }

    AudioSystem::~AudioSystem() {
        shutdown();
    }

    bool AudioSystem::init([[maybe_unused]] int sampleRate, [[maybe_unused]] int channels) {
#ifdef NOX_HAS_SDL3_MIXER
        if (initialized_) { return true; }

        if (!MIX_Init()) {
            NOX_LOG_ERROR("Failed to init SDL3_mixer: {}", SDL_GetError());
            return false;
        }

        // Create a mixer device with default settings
        SDL_AudioSpec spec{};
        spec.freq = sampleRate;
        spec.format = SDL_AUDIO_S16;
        spec.channels = channels;

        auto* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
        if (!mixer) {
            NOX_LOG_ERROR("Failed to create mixer device: {}", SDL_GetError());
            MIX_Quit();
            return false;
        }
        mixer_ = mixer;

        // Pre-create tracks
        for (int i = 0; i < MaxTracks; ++i) {
            tracks_[static_cast<size_t>(i)] = MIX_CreateTrack(mixer);
        }

        initialized_ = true;
        NOX_LOG_INFO("Audio system initialized (SDL3_mixer)");
        return true;
#else
        NOX_LOG_WARN("Audio system not available (SDL3_mixer not linked)");
        return false;
#endif
    }

    void AudioSystem::shutdown() {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_) { return; }

        // Destroy tracks
        for (int i = 0; i < MaxTracks; ++i) {
            auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(i)]);
            if (track) {
                MIX_DestroyTrack(track);
                tracks_[static_cast<size_t>(i)] = nullptr;
            }
        }

        // Free all loaded clips
        for (auto& [id, audio] : clips_) {
            MIX_DestroyAudio(static_cast<MIX_Audio*>(audio));
        }
        clips_.clear();

        // Destroy mixer
        if (mixer_) {
            MIX_DestroyMixer(static_cast<MIX_Mixer*>(mixer_));
            mixer_ = nullptr;
        }

        MIX_Quit();
        initialized_ = false;
        NOX_LOG_INFO("Audio system shut down");
#endif
    }

    AudioClipHandle AudioSystem::loadClip([[maybe_unused]] const std::filesystem::path& path,
                                           [[maybe_unused]] bool streaming) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_) { return {}; }

        auto* mixer = static_cast<MIX_Mixer*>(mixer_);
        // predecode=true for sound effects (fast playback), false for streaming music
        MIX_Audio* audio = MIX_LoadAudio(mixer, path.string().c_str(), !streaming);
        if (!audio) {
            NOX_LOG_ERROR("Failed to load audio '{}': {}", path.string(), SDL_GetError());
            return {};
        }

        uint32_t id = nextClipId_++;
        clips_[id] = audio;
        NOX_LOG_INFO("Loaded audio '{}' (id={}, streaming={})", path.filename().string(), id, streaming);
        return { id };
#else
        return {};
#endif
    }

    int AudioSystem::play([[maybe_unused]] AudioClipHandle clip,
                           [[maybe_unused]] const AudioSourceConfig& config) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_ || !clip.valid()) { return -1; }

        auto it = clips_.find(clip.id);
        if (it == clips_.end()) { return -1; }

        // Find a free track
        int trackIndex = -1;
        for (int i = 0; i < MaxTracks; ++i) {
            if (!trackStates_[static_cast<size_t>(i)].active) {
                trackIndex = i;
                break;
            }
        }
        if (trackIndex < 0) { return -1; }

        auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(trackIndex)]);
        auto* audio = static_cast<MIX_Audio*>(it->second);

        if (!MIX_SetTrackAudio(track, audio)) {
            NOX_LOG_ERROR("Failed to set track audio: {}", SDL_GetError());
            return -1;
        }

        // Set loops
        if (config.loop) {
            MIX_SetTrackLoops(track, -1); // infinite
        }

        // Set gain
        MIX_SetTrackGain(track, config.volume);

        // Play
        if (!MIX_PlayTrack(track, 0)) {
            NOX_LOG_ERROR("Failed to play track: {}", SDL_GetError());
            return -1;
        }

        auto& state = trackStates_[static_cast<size_t>(trackIndex)];
        state.config = config;
        state.active = true;

        return trackIndex;
#else
        return -1;
#endif
    }

    void AudioSystem::stopTrack([[maybe_unused]] int trackIndex,
                                 [[maybe_unused]] float fadeOutMs) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_ || trackIndex < 0 || trackIndex >= MaxTracks) { return; }

        auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(trackIndex)]);
        if (!track) { return; }

        Sint64 fadeFrames = 0;
        if (fadeOutMs > 0.0f) {
            fadeFrames = MIX_MSToFrames(44100, static_cast<Sint64>(fadeOutMs));
        }
        MIX_StopTrack(track, fadeFrames);
        trackStates_[static_cast<size_t>(trackIndex)].active = false;
#endif
    }

    void AudioSystem::stopAll([[maybe_unused]] float fadeOutMs) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_) { return; }
        auto* mixer = static_cast<MIX_Mixer*>(mixer_);
        MIX_StopAllTracks(mixer, static_cast<Sint64>(fadeOutMs));
        for (auto& state : trackStates_) { state.active = false; }
#endif
    }

    void AudioSystem::pauseTrack([[maybe_unused]] int trackIndex) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_ || trackIndex < 0 || trackIndex >= MaxTracks) { return; }
        auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(trackIndex)]);
        if (track) { MIX_PauseTrack(track); }
#endif
    }

    void AudioSystem::resumeTrack([[maybe_unused]] int trackIndex) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_ || trackIndex < 0 || trackIndex >= MaxTracks) { return; }
        auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(trackIndex)]);
        if (track) { MIX_ResumeTrack(track); }
#endif
    }

    void AudioSystem::setTrackGain([[maybe_unused]] int trackIndex,
                                    [[maybe_unused]] float gain) {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_ || trackIndex < 0 || trackIndex >= MaxTracks) { return; }
        auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(trackIndex)]);
        if (track) { MIX_SetTrackGain(track, gain); }
#endif
    }

    void AudioSystem::setMasterGain([[maybe_unused]] float gain) {
#ifdef NOX_HAS_SDL3_MIXER
        masterGain_ = gain;
        if (initialized_ && mixer_) {
            MIX_SetMixerGain(static_cast<MIX_Mixer*>(mixer_), gain);
        }
#else
        masterGain_ = gain;
#endif
    }

    void AudioSystem::setListener(const AudioListener& listener) {
        listener_ = listener;
    }

    void AudioSystem::setTrackPosition3D([[maybe_unused]] int trackIndex,
                                          [[maybe_unused]] const Math::Vec3& position,
                                          [[maybe_unused]] const Math::Vec3& velocity) {
        if (trackIndex < 0 || trackIndex >= MaxTracks) { return; }
        auto& state = trackStates_[static_cast<size_t>(trackIndex)];
        state.position = position;
        state.velocity = velocity;
    }

    void AudioSystem::addReverbZone(const ReverbZone& zone) {
        reverbZones_.push_back(zone);
    }

    void AudioSystem::clearReverbZones() {
        reverbZones_.clear();
    }

    void AudioSystem::update() {
#ifdef NOX_HAS_SDL3_MIXER
        if (!initialized_) { return; }

        for (int i = 0; i < MaxTracks; ++i) {
            auto& state = trackStates_[static_cast<size_t>(i)];
            if (!state.active || !state.config.spatial) { continue; }
            applySpatialEffect(i);
        }
#endif
    }

    void AudioSystem::applySpatialEffect([[maybe_unused]] int trackIndex) {
#ifdef NOX_HAS_SDL3_MIXER
        auto& state = trackStates_[static_cast<size_t>(trackIndex)];

        // Compute direction and distance from listener to source
        Math::Vec3 toSource = state.position - listener_.position;
        float distance = glm::length(toSource);

        // Distance attenuation (linear falloff between min/max distance)
        float attenuation = 1.0f;
        if (distance > state.config.minDistance) {
            if (distance >= state.config.maxDistance) {
                attenuation = 0.0f;
            }
            else {
                attenuation = 1.0f - (distance - state.config.minDistance) /
                              (state.config.maxDistance - state.config.minDistance);
            }
        }

        // Apply volume attenuation through track gain
        float gain = state.config.volume * attenuation;
        auto* track = static_cast<MIX_Track*>(tracks_[static_cast<size_t>(trackIndex)]);
        if (track) {
            MIX_SetTrackGain(track, gain);
        }
#endif
    }

} // namespace Nox
