#include "Enemy/EnemyManager.hpp"

#include <cstdlib>

#include "GameManager.hpp"
#include "Item/ItemManager.hpp"
#include "Player.hpp"
#include "Util/Math.hpp"

static constexpr ItemType RANDOM_ITEM_TABLE[32] = {
    ItemType::PowerSmall, ItemType::PowerSmall, ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::PowerSmall, ItemType::Point,
    ItemType::Point,      ItemType::Point,      ItemType::PowerSmall, ItemType::PowerSmall,
    ItemType::PowerSmall, ItemType::Point,      ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::PowerSmall, ItemType::Point,
    ItemType::Point,      ItemType::Point,      ItemType::PowerSmall, ItemType::PowerBig,
};

EnemyManager::EnemyManager() = default;

void EnemyManager::SetScript(std::unique_ptr<IStageScript> script) {
    m_Script = std::move(script);
    if (m_Script) m_Script->Preload(m_Anm);
}

EnemySubCtx EnemyManager::MakeCtx() {
    return EnemySubCtx{m_Anm, m_BulletManager, m_LaserManager, *m_Items, m_Renderer, m_PlayerPos};
}

void EnemyManager::UpdateBossCallbacks(Enemy& enemy, GameManager& /*gm*/) {
    if (!enemy.m_IsBoss) return;

    enemy.m_BossTimer++;

    if (enemy.m_LifeCallbackThreshold >= 0 && enemy.m_Life < enemy.m_LifeCallbackThreshold) {
        enemy.m_Life                  = enemy.m_LifeCallbackThreshold;
        int sub                       = enemy.m_LifeCallbackSub;
        enemy.m_LifeCallbackThreshold = -1;
        enemy.m_LifeCallbackSub       = -1;
        enemy.m_TimerCallbackSub      = enemy.m_DeathCallbackSub;
        for (auto& e : m_Enemies) {
            if (e.m_Alive && !e.m_IsBoss) e.m_Life = 0;
        }
        enemy.m_SubId      = sub;
        enemy.m_FrameTimer = -1;
        return;
    }

    if (enemy.m_TimerCallbackThreshold >= 0 &&
        enemy.m_BossTimer >= enemy.m_TimerCallbackThreshold) {
        if (enemy.m_LifeCallbackThreshold > 0) {
            enemy.m_Life                  = enemy.m_LifeCallbackThreshold;
            enemy.m_LifeCallbackThreshold = -1;
        }
        int sub                        = enemy.m_TimerCallbackSub;
        enemy.m_TimerCallbackThreshold = -1;
        enemy.m_TimerCallbackSub       = enemy.m_DeathCallbackSub;
        m_BulletManager.ClearAll();
        m_LaserManager.ClearAll();
        for (auto& e : m_Enemies) {
            if (e.m_Alive && !e.m_IsBoss) e.m_Life = 0;
        }
        enemy.m_SubId      = sub;
        enemy.m_FrameTimer = -1;
        return;
    }
}

Enemy* EnemyManager::SpawnEnemy(int subId, float x, float y, int life, int score, bool mirrored,
                                int itemDrop) {
    for (auto& enemy : m_Enemies) {
        if (enemy.m_Alive) continue;

        enemy              = Enemy{};
        enemy.m_Alive      = true;
        enemy.m_SubId      = subId;
        enemy.m_Pos        = Util::GameFieldToScreen(x, y);
        enemy.m_Life       = life;
        enemy.m_Score      = score;
        enemy.m_Mirrored   = mirrored;
        enemy.m_FrameTimer = 0;

        if (m_Script) {
            auto ctx = MakeCtx();
            m_Script->InitSub(enemy, ctx);
        }

        if (itemDrop != -99) {
            enemy.m_ItemDrop = itemDrop;
        }

        if (enemy.m_Vm.obj) {
            m_Renderer.AddChild(enemy.m_Vm.obj);
        }
        return &enemy;
    }
    return nullptr;
}

void EnemyManager::UpdatePhysics(Enemy& enemy) {
    if (enemy.m_IsLerping) {
        enemy.m_LerpElapsed++;
        float t     = 1.0f - (float)enemy.m_LerpElapsed / (float)enemy.m_LerpFrames;
        float frac  = 1.0f - t * t;
        enemy.m_Pos = enemy.m_LerpOrigin + frac * (enemy.m_LerpTarget - enemy.m_LerpOrigin);
        if (enemy.m_LerpElapsed >= enemy.m_LerpFrames) {
            enemy.m_Pos       = enemy.m_LerpTarget;
            enemy.m_IsLerping = false;
        }
        return;
    }
    enemy.m_Pos.x += std::cos(enemy.m_Angle) * enemy.m_Speed;
    enemy.m_Pos.y += std::sin(enemy.m_Angle) * enemy.m_Speed;
    enemy.m_Angle += enemy.m_AngularVelocity;
    enemy.m_Speed += enemy.m_Acceleration;
}

void EnemyManager::SetTimeline(const TimelineEntry* entries, int count) {
    m_Timeline     = entries;
    m_TimelineSize = count;
    m_TimelineIdx  = 0;
}

void EnemyManager::RunTimeline() {
    if (!m_Timeline) return;
    for (auto& e : m_Enemies) {
        if (e.m_Alive && e.m_BlocksTimeline) return;
    }
    while (m_TimelineIdx < m_TimelineSize && m_Timeline[m_TimelineIdx].frame <= m_Frame) {
        const auto& e = m_Timeline[m_TimelineIdx];
        if (e.frame == m_Frame) {
            float spawnX = e.randomX ? static_cast<float>(rand() % 353 + 16) : e.x;
            SpawnEnemy(e.subId, spawnX, e.y, e.life, e.score, e.mirrored, e.itemDrop);
        }
        m_TimelineIdx++;
    }
}

void EnemyManager::Update(const glm::vec2& playerPos, GameManager& gm) {
    m_PlayerPos = playerPos;
    RunTimeline();

    auto ctx = MakeCtx();
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;

        UpdateBossCallbacks(enemy, gm);
        if (!enemy.m_Alive) continue;

        if (m_Script) m_Script->RunSub(enemy, ctx);

        if (!enemy.m_Alive) {
            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
            continue;
        }

        UpdatePhysics(enemy);

        enemy.m_Vm.pos = enemy.m_Pos;
        m_Anm.UpdateObjects(enemy.m_Vm);

        enemy.m_FrameTimer++;

        if (!enemy.m_IsBoss &&
            !Util::IsInGameBounds(enemy.m_Pos.x, enemy.m_Pos.y, 0, 0, -200, -200, 600, 700)) {
            enemy.m_Alive = false;
            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
        }
    }

    m_Renderer.Update();
    m_BulletManager.Update();
    m_LaserManager.Update();
    m_Frame++;
}

int EnemyManager::ApplyPlayerBulletDamage(Player& player) {
    int totalScore = 0;
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;
        if (enemy.m_IsBoss && !enemy.m_CanTakeDamage) continue;

        int dmg = player.CalcDamageToEnemy(enemy.m_Pos, enemy.m_HitboxSize);
        if (dmg <= 0) continue;
        enemy.m_Life -= dmg;

        if (enemy.m_Life <= 0) {
            if (enemy.m_IsBoss) {
                enemy.m_Life             = 0;
                enemy.m_CanTakeDamage    = false;
                int sub                  = enemy.m_DeathCallbackSub;
                enemy.m_DeathCallbackSub = -1;
                if (sub >= 0) {
                    m_BulletManager.ClearAll();
                    m_LaserManager.ClearAll();
                    enemy.m_SubId      = sub;
                    enemy.m_FrameTimer = -1;
                } else {
                    totalScore += enemy.m_Score;
                    enemy.m_Alive = false;
                    if (enemy.m_Vm.obj) {
                        m_Renderer.RemoveChild(enemy.m_Vm.obj);
                        enemy.m_Vm.obj = nullptr;
                    }
                }
            } else {
                enemy.m_Alive = false;
                totalScore += enemy.m_Score;

                for (int k = 0; k < enemy.m_ItemDropCount; k++) {
                    if (enemy.m_ItemDrop >= 0) {
                        m_Items->SpawnItem(enemy.m_Pos, static_cast<ItemType>(enemy.m_ItemDrop));
                    } else {
                        if (m_RandomItemSpawnIndex++ % 3 == 0) {
                            m_Items->SpawnItem(enemy.m_Pos,
                                               RANDOM_ITEM_TABLE[m_RandomItemTableIndex]);
                            m_RandomItemTableIndex = (m_RandomItemTableIndex + 1) % 32;
                        }
                    }
                }

                if (enemy.m_Vm.obj) {
                    m_Renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
        }
    }
    return totalScore;
}

bool EnemyManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    return m_BulletManager.CheckPlayerHit(playerPos, playerHitboxSize) ||
           m_LaserManager.CheckPlayerHit(playerPos, playerHitboxSize);
}

void EnemyManager::ClearAllBullets() {
    m_BulletManager.ClearAll();
    m_LaserManager.ClearAll();
}
