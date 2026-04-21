// Copyright (c) 2026 NoxFly — AGPL-3.0
#pragma once

#include <NoxEngine/math/Types.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Nox {

    /// Opaque handle to a loaded audio clip.
    struct AudioClipHandle {
        uint32_t id = 0;
        [[nodiscard]] bool valid() const { return id != 0; }
    };

    /// 3D audio listener (usually attached to the camera).
    struct AudioListener {
        Math::Vec3 position  = { 0.0f, 0.0f, 0.0f };
        Math::Vec3 forward   = { 0.0f, 0.0f, -1.0f };
        Math::Vec3 up        = { 0.0f, 1.0f, 0.0f };
        Math::Vec3 velocity  = { 0.0f, 0.0f, 0.0f };
    };

    /// Configuration for a playing audio source.
    struct AudioSourceConfig {
        float volume       = 1.0f;    ///< 0..1
        float pitch        = 1.0f;    ///< Playback speed multiplier
        bool  loop         = false;
        bool  spatial      = false;   ///< Enable 3D spatialization
        float minDistance   = 1.0f;   ///< Distance at which volume starts to decrease
        float maxDistance   = 100.0f; ///< Distance at which sound is inaudible
        float dopplerFactor = 1.0f;   ///< Doppler effect strength
    };

    /// Reverb zone for environmental audio effects.
    struct ReverbZone {
        Math::Vec3 position = { 0.0f, 0.0f, 0.0f };
        float radius        = 10.0f;
        float decay         = 1.5f;     ///< Reverb decay time in seconds
        float density       = 0.5f;     ///< 0..1
        float diffusion     = 0.5f;     ///< 0..1
    };

    /// Central audio system built on SDL3_mixer.
    /// Manages clip loading, playback, 3D spatialization, and reverb zones.
    class AudioSystem {
    public:
        AudioSystem();
        ~AudioSystem();

        AudioSystem(const AudioSystem&) = delete;
        AudioSystem& operator=(const AudioSystem&) = delete;
        AudioSystem(AudioSystem&&) = delete;
        AudioSystem& operator=(AudioSystem&&) = delete;

        /// Initialize the audio subsystem. Must be called before any audio operations.
        [[nodiscard]] bool init(int sampleRate = 44100, int channels = 2);

        /// Shutdown the audio subsystem.
        void shutdown();

        /// Load an audio clip from file (WAV, OGG, MP3, FLAC).
        /// If streaming is true, audio is decoded on-the-fly (for music).
        [[nodiscard]] AudioClipHandle loadClip(const std::filesystem::path& path,
                                                bool streaming = false);

        /// Play a loaded clip. Returns a track index (or -1 on failure).
        int play(AudioClipHandle clip, const AudioSourceConfig& config = {});

        /// Stop a specific track.
        void stopTrack(int trackIndex, float fadeOutMs = 0.0f);

        /// Stop all tracks.
        void stopAll(float fadeOutMs = 0.0f);

        /// Pause / resume a track.
        void pauseTrack(int trackIndex);
        void resumeTrack(int trackIndex);

        /// Set gain for a track (0..∞, 1.0 = normal).
        void setTrackGain(int trackIndex, float gain);

        /// Set master gain (0..∞, 1.0 = normal).
        void setMasterGain(float gain);

        // ── 3D Audio ───────────────────────────────────────────────
        /// Update the listener state (call once per frame, usually from camera).
        void setListener(const AudioListener& listener);

        /// Update a track's 3D position for spatialization.
        void setTrackPosition3D(int trackIndex, const Math::Vec3& position,
                                 const Math::Vec3& velocity = Math::Vec3(0.0f));

        // ── Reverb zones ───────────────────────────────────────────
        void addReverbZone(const ReverbZone& zone);
        void clearReverbZones();

        /// Per-frame update: applies 3D spatialization based on listener + source positions.
        void update();

        [[nodiscard]] bool isInitialized() const { return initialized_; }

    private:
        static constexpr int MaxTracks = 64;

        struct TrackState {
            Math::Vec3 position = { 0.0f, 0.0f, 0.0f };
            Math::Vec3 velocity = { 0.0f, 0.0f, 0.0f };
            AudioSourceConfig config;
            bool active = false;
        };

        void applySpatialEffect(int trackIndex);

        bool initialized_ = false;
        AudioListener listener_;
        float masterGain_ = 1.0f;

        uint32_t nextClipId_ = 1;
        std::unordered_map<uint32_t, void*> clips_;  ///< MIX_Audio pointers

        void* mixer_ = nullptr;                       ///< MIX_Mixer pointer
        std::array<void*, MaxTracks> tracks_{};        ///< MIX_Track pointers
        std::array<TrackState, MaxTracks> trackStates_{};

        std::vector<ReverbZone> reverbZones_;
    };

} // namespace Nox
