#include "Audio/AudioManager.hpp"

#include <SDL_mixer.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>

#include "Util/JsonConfigLoader.hpp"
#include "Util/Logger.hpp"

namespace {
struct SoundConfig {
    const char* path;
    int         directSoundVolume;
};

constexpr SoundConfig SOUND_CONFIGS[] = {
    {"th06c/th06c_IN/plst00.wav", -1500},   {"th06c/th06c_IN/enep00.wav", -1400},
    {"th06c/th06c_IN/pldead00.wav", -1000}, {"th06c/th06c_IN/ok00.wav", -1000},
    {"th06c/th06c_IN/cancel00.wav", -1000}, {"th06c/th06c_IN/select00.wav", -1900},
    {"th06c/th06c_IN/nep00.wav", -400},     {"th06c/th06c_IN/item00.wav", -900},
    {"th06c/th06c_IN/extend.wav", -100},    {"th06c/th06c_IN/powerup.wav", -1000},
    {"th06c/th06c_IN/damage00.wav", -1100}, {"th06c/th06c_IN/cat00.wav", -900},
    {"th06c/th06c_IN/lazer00.wav", -900},   {"th06c/th06c_IN/tan00.wav", -1700},
    {"th06c/th06c_IN/graze.wav", -1000},
};

struct MusicLoopPoints {
    double startSeconds = 0.0;
    double endSeconds   = 0.0;
};

constexpr double ORIGINAL_BGM_SAMPLE_RATE = 44100.0;

MusicLoopPoints LoadMusicLoopPoints(const std::string& relativePath) {
    const size_t extension = relativePath.find_last_of('.');
    if (extension == std::string::npos) return {};

    const std::string posPath =
        Util::JsonConfig::ResourcePath(relativePath.substr(0, extension) + ".pos");
    std::ifstream file(posPath, std::ios::binary);
    if (!file) return {};

    std::uint32_t startFrame = 0;
    std::uint32_t endFrame   = 0;
    file.read(reinterpret_cast<char*>(&startFrame), sizeof(startFrame));
    file.read(reinterpret_cast<char*>(&endFrame), sizeof(endFrame));
    if (!file || endFrame <= startFrame) return {};

    return {
        static_cast<double>(startFrame) / ORIGINAL_BGM_SAMPLE_RATE,
        static_cast<double>(endFrame) / ORIGINAL_BGM_SAMPLE_RATE,
    };
}

static_assert(static_cast<int>(SoundEffect::Count) ==
              static_cast<int>(sizeof(SOUND_CONFIGS) / sizeof(SOUND_CONFIGS[0])));

int DirectSoundVolumeToMixerVolume(int volume) {
    const double gain = std::pow(10.0, static_cast<double>(volume) / 2000.0);
    return std::clamp(static_cast<int>(std::lround(MIX_MAX_VOLUME * gain)), 0, MIX_MAX_VOLUME);
}

std::shared_ptr<Mix_Chunk> LoadSound(const SoundConfig& config) {
    const std::string path  = Util::JsonConfig::ResourcePath(config.path);
    auto              sound = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(path.c_str()), Mix_FreeChunk);
    if (!sound) LOG_DEBUG("Failed to load SFX '{}': {}", path, Mix_GetError());
    if (sound)
        Mix_VolumeChunk(sound.get(), DirectSoundVolumeToMixerVolume(config.directSoundVolume));
    return sound;
}

std::shared_ptr<Mix_Music> LoadMusic(const std::string& relativePath) {
    const std::string path  = Util::JsonConfig::ResourcePath(relativePath);
    auto              music = std::shared_ptr<Mix_Music>(Mix_LoadMUS(path.c_str()), Mix_FreeMusic);
    if (!music) LOG_ERROR("Failed to load BGM '{}': {}", path, Mix_GetError());
    return music;
}
}  // namespace

AudioManager& AudioManager::Instance() {
    static auto* manager = new AudioManager();
    return *manager;
}

AudioManager::AudioManager() {
    m_PendingSounds.fill(-1);
    Mix_AllocateChannels(std::max(Mix_AllocateChannels(-1), SOUND_EFFECT_COUNT));
    for (int i = 0; i < SOUND_EFFECT_COUNT; ++i) {
        m_Sounds[i] = LoadSound(SOUND_CONFIGS[i]);
    }
}

void AudioManager::Tick() {
    ++m_Frame;
    if (m_Music && m_MusicLoopEndSeconds > 0.0 &&
        Mix_GetMusicPosition(m_Music.get()) >= m_MusicLoopEndSeconds) {
        if (Mix_SetMusicPosition(m_MusicLoopStartSeconds) < 0) {
            LOG_ERROR("Failed to seek BGM loop: {}", Mix_GetError());
            m_MusicLoopEndSeconds = 0.0;
        }
    }
}

void AudioManager::Flush() {
    for (int& idx : m_PendingSounds) {
        if (idx < 0) break;
        if (m_Sounds[idx]) {
            Mix_HaltChannel(idx);
            Mix_PlayChannel(idx, m_Sounds[idx].get(), 0);
        }
        idx = -1;
    }
}

void AudioManager::Play(SoundEffect effect, int /*minIntervalFrames*/) {
    const int idx = static_cast<int>(effect);
    if (!m_Sounds[idx]) return;

    for (int& pending : m_PendingSounds) {
        if (pending == idx) return;
        if (pending >= 0) continue;

        pending = idx;
        return;
    }
}

void AudioManager::PlayMusic(const std::string& relativePath, int fadeInMs) {
    if (relativePath.empty() || relativePath == m_MusicPath) return;

    auto music = LoadMusic(relativePath);
    if (!music) return;

    Mix_HaltMusic();
    m_Music                 = std::move(music);
    m_MusicPath             = relativePath;
    const auto loopPoints   = LoadMusicLoopPoints(relativePath);
    m_MusicLoopStartSeconds = loopPoints.startSeconds;
    m_MusicLoopEndSeconds   = loopPoints.endSeconds;
    const int loops         = m_MusicLoopEndSeconds > 0.0 ? 0 : -1;
    const int result        = fadeInMs > 0 ? Mix_FadeInMusic(m_Music.get(), loops, fadeInMs)
                                           : Mix_PlayMusic(m_Music.get(), loops);
    if (result < 0) LOG_ERROR("Failed to play BGM '{}': {}", relativePath, Mix_GetError());
}

void AudioManager::StopMusic(int fadeOutMs) {
    if (fadeOutMs > 0) {
        Mix_FadeOutMusic(fadeOutMs);
    } else {
        Mix_HaltMusic();
        m_Music.reset();
    }
    m_MusicPath.clear();
    m_MusicLoopStartSeconds = 0.0;
    m_MusicLoopEndSeconds   = 0.0;
}
