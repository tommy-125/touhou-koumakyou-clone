#ifndef ENEMY_MANAGER_HPP
#define ENEMY_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>

#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Util/Renderer.hpp"

class Player;

// Timeline entry for hand-coded stage script
struct TimelineEntry {
    int   frame;
    int   subId;
    float x, y;
    int   life;
    int   score;
    bool  mirrored;
};

class EnemyManager {
   public:
    EnemyManager();
    void Update(const glm::vec2& playerPos);
    void ApplyPlayerBulletDamage(Player& player);
    bool CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize);
    void ClearAllBullets();

   private:
    static constexpr int MAX_ENEMIES = 256;

    Enemy* SpawnEnemy(int subId, float x, float y, int life, int score, bool mirrored = false);
    void   InitSub(Enemy& enemy);
    void   RunSub(Enemy& enemy);
    void   UpdatePhysics(Enemy& enemy);
    void   RunTimeline();

    std::array<Enemy, MAX_ENEMIES> m_Enemies{};

    Anm::Manager       m_Anm;
    Util::Renderer     m_Renderer;
    EnemyBulletManager m_BulletManager;

    int       m_Frame       = 0;
    int       m_TimelineIdx = 0;
    glm::vec2 m_PlayerPos   = {192, 384};
};

#endif  // ENEMY_MANAGER_HPP
