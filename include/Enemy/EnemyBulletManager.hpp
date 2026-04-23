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
    glm::vec2 m_Pos          = {0, 0};
    float     m_Angle        = 0;
    float     m_Speed        = 0;
    glm::vec2 m_HitboxSize   = {5, 5};
    float     m_Acceleration = 0.0f;
    bool      m_Alive        = false;
    bool      m_UseDecay     = false;
    int       m_DecayTimer   = 0;
    // TH06 auto-rotate style for angle-aware bullets.
    bool m_RotateWithAngle = false;
    // Direction change (ECL flag 0x40 / ins_82): every m_DirChangeInterval frames,
    // rotate to new direction and restore speed, up to m_DirChangeMaxTimes.
    int   m_DirChangeInterval = -1;
    int   m_DirChangeNumTimes = 0;
    int   m_DirChangeMaxTimes = 0;
    float m_DirChangeAngle    = 0.0f;
    float m_DirChangeSpeed    = 0.0f;
    bool  m_DirChangeRelative = false;
};

// Direction change (ECL flag 0x40 / ins_82): every `at` frames, rotate/re-speed.
// relative=true: new angle = current angle + angle; false: absolute world angle.
struct BulletCurve {
    int   at       = -1;
    float angle    = 0.0f;
    float speed    = 0.0f;
    bool  relative = false;
    int   times    = 1;
};

class EnemyBulletManager {
   public:
    EnemyBulletManager();

    void SpawnFanAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type, EBulletColor color,
                       int count, float speed, float aimOffset, float spread, bool useDecay = false,
                       bool rotateWithAngle = false);
    // ECL-style: speed1 = outermost ring, speed2 = innermost ring (speed1 → speed2 linearly)
    void SpawnFanStack(glm::vec2 pos, glm::vec2 playerPos, EBulletType type, EBulletColor color,
                       int ways, int stacks, float speed1, float speed2, float aimOffset,
                       float spread);
    // CIRCLE_AIMED: full 360° ring rotated toward player + aimOffset
    void SpawnCircleAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type, EBulletColor color,
                          int count, float speed, float aimOffset = 0.0f, bool useDecay = false,
                          float acceleration = 0.0f, BulletCurve curve = {},
                          bool rotateWithAngle = false);
    // CIRCLE: full 360° ring at absolute baseAngle (no player aiming)
    void SpawnCircle(glm::vec2 pos, EBulletType type, EBulletColor color, int count, float speed,
                     float baseAngle = 0.0f, bool useDecay = false);

    void Update();
    bool CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize);
    void ClearAll();

    static constexpr int MAX_BULLETS = 640;

   private:
    EnemyBullet* AllocBullet();

    std::array<EnemyBullet, MAX_BULLETS> m_Bullets{};
    int                                  m_NextIdx = 0;
    Anm::Manager                         m_Anm;
    Util::Renderer                       m_Renderer;
};

#endif  // ENEMY_BULLET_MANAGER_HPP
