#ifndef ENEMY_MANAGER_HPP
#define ENEMY_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>
#include <memory>

#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/IStageScript.hpp"
#include "Scene/Timeline.hpp"
#include "Util/Renderer.hpp"

struct GameManager;
class ItemManager;
class Player;

class EnemyManager {
   public:
    EnemyManager();
    void SetTimeline(const TimelineEntry* entries, int count);
    void SetScript(std::unique_ptr<IStageScript> script);
    void SetItemManager(ItemManager* items) { m_Items = items; }
    void Update(const glm::vec2& playerPos, GameManager& gm);
    int  ApplyPlayerBulletDamage(Player& player);
    bool CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize);
    void ClearAllBullets();

   private:
    static constexpr int MAX_ENEMIES = 256;

    Enemy*      SpawnEnemy(int subId, float x, float y, int life, int score, bool mirrored = false,
                           int itemDrop = -99);
    void        UpdatePhysics(Enemy& enemy);
    void        RunTimeline();
    void        UpdateBossCallbacks(Enemy& enemy, GameManager& gm);
    EnemySubCtx MakeCtx();

    std::array<Enemy, MAX_ENEMIES> m_Enemies{};

    Anm::Manager       m_Anm;
    Util::Renderer     m_Renderer;
    EnemyBulletManager m_BulletManager;
    EnemyLaserManager  m_LaserManager;
    ItemManager*       m_Items = nullptr;

    std::unique_ptr<IStageScript> m_Script;

    const TimelineEntry* m_Timeline     = nullptr;
    int                  m_TimelineSize = 0;
    int                  m_TimelineIdx  = 0;

    int       m_Frame                = 0;
    glm::vec2 m_PlayerPos            = {192, 384};
    int       m_RandomItemSpawnIndex = 0;
    int       m_RandomItemTableIndex = 0;
};

#endif  // ENEMY_MANAGER_HPP
