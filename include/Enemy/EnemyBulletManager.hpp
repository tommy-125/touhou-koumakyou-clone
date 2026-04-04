#ifndef ENEMY_BULLET_MANAGER_HPP
#define ENEMY_BULLET_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>

#include "Anm/AnmManager.hpp"
#include "Util/Renderer.hpp"

enum class EBulletType {
    Pellet   = 0,
    RingBall = 1,
    Rice     = 2,
    Ball     = 3,
    Kunai    = 4,
    Shard    = 5,
    BigBall  = 6,
    Fireball = 7,
    Dagger   = 8,
    Bubble   = 9,
};

enum class EBulletColor {
    Gray       = 0,
    DarkRed    = 1,
    Red        = 2,
    DarkPurple = 3,
    Purple     = 4,
    DarkBlue   = 5,
    Blue       = 6,
    DarkCyan   = 7,
    Cyan       = 8,
    DarkGreen  = 9,
    Green      = 10,
    Lime       = 11,
    DarkYellow = 12,
    Yellow     = 13,
    Orange     = 14,
    White      = 15,
};

struct EnemyBullet {
    Anm::Vm   m_Vm;
    glm::vec2 m_Pos        = {0, 0};
    float     m_Angle      = 0;
    float     m_Speed      = 0;
    glm::vec2 m_HitboxSize = {5, 5};
    bool      m_Alive      = false;
};

class EnemyBulletManager {
   public:
    EnemyBulletManager();

    void SpawnFanAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type, EBulletColor color,
                       int count, float speed, float aimOffset, float spread);

    void Update();

    static constexpr int MAX_BULLETS = 640;

   private:
    EnemyBullet* AllocBullet();

    std::array<EnemyBullet, MAX_BULLETS> m_Bullets{};
    int                                  m_NextIdx = 0;
    Anm::Manager                         m_Anm;
    Util::Renderer                       m_Renderer;
};

#endif  // ENEMY_BULLET_MANAGER_HPP
