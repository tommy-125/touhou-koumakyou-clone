#ifndef AUDIO_AUDIO_MANAGER_HPP
#define AUDIO_AUDIO_MANAGER_HPP

#include <array>
#include <memory>
#include <string>

struct Mix_Chunk;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;

enum class SoundEffect {
    PlayerShoot,
    EnemyDeath,
    PlayerDeath,
    MenuConfirm,
    MenuBack,
    MenuMove,
    Bomb,
    ItemPickup,
    Extend,
    PowerUp,
    BossDamage,
    SpellStart,
    Laser,
    EnemyShoot,
    Graze,
    Count,
};

class AudioManager {
   public:
    static AudioManager& Instance();

    void Tick();
    void Flush();
    void Play(SoundEffect effect, int minIntervalFrames = 0);
    void PlayMusic(const std::string& relativePath, int fadeInMs = 0);
    void StopMusic(int fadeOutMs = 0);

   private:
    AudioManager();

    static constexpr int SOUND_EFFECT_COUNT = static_cast<int>(SoundEffect::Count);
    static constexpr int PENDING_SOUND_COUNT = 3;

    std::array<std::shared_ptr<Mix_Chunk>, SOUND_EFFECT_COUNT> m_Sounds;
    std::array<int, PENDING_SOUND_COUNT>                       m_PendingSounds;
    std::shared_ptr<Mix_Music>                                 m_Music;
    std::string                                                m_MusicPath;
    int                                                        m_Frame = 0;
    double                                                     m_MusicLoopStartSeconds = 0.0;
    double                                                     m_MusicLoopEndSeconds   = 0.0;
};

#endif  // AUDIO_AUDIO_MANAGER_HPP
