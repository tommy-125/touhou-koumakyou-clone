#include "Enemy/EnemyManager.hpp"

#include <cmath>

#include "Anm/AnmDefs.hpp"
#include "GameManager.hpp"
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
    ItemType::Point,      ItemType::Point,      ItemType::PowerSmall, ItemType::Point,
};

// ── Construction ─────────────────────────────────────────────────────────────

EnemyManager::EnemyManager() {
    // stg1enm (small/medium fairies) and stg1enm2 (boss) share the same offset
    m_Anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    m_Anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

// ── Spawning ─────────────────────────────────────────────────────────────────

Enemy* EnemyManager::SpawnEnemy(int subId, float x, float y, int life, int score, bool mirrored) {
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

        InitSub(enemy);

        if (enemy.m_Vm.obj) {
            m_Renderer.AddChild(enemy.m_Vm.obj);
        }
        return &enemy;
    }
    return nullptr;  // pool full
}

// ── Sub initialization (frame-0 actions) ─────────────────────────────────────

void EnemyManager::InitSub(Enemy& enemy) {
    int offset = Anm::STG1ENM.offset;

    switch (enemy.m_SubId) {
        case 0:  // Small fairy: move down + curve
            m_Anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 1:  // Small fairy variant
            m_Anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 2:  // Medium fairy: stops and shoots
            m_Anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;  // PowerSmall (ECL PowerItem = ITEM_POWER_SMALL)
            break;

        case 3:  // Medium fairy: stops (no bullet on Normal)
            m_Anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;  // PowerSmall (ECL PowerItem = ITEM_POWER_SMALL)
            break;
    }
}

// ── Sub execution (timed events per frame) ───────────────────────────────────

void EnemyManager::RunSub(Enemy& enemy) {
    int t      = enemy.m_FrameTimer;
    int offset = Anm::STG1ENM.offset;

    float dir = enemy.m_Mirrored ? -1.0f : 1.0f;

    switch (enemy.m_SubId) {
        case 0:
            if (t == 40) enemy.m_AngularVelocity = dir * -0.024543693f;
            if (t == 120) enemy.m_AngularVelocity = dir * 0.019634955f;
            if (t == 220) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 1:
            if (t == 100) enemy.m_AngularVelocity = dir * 0.019634955f;
            if (t == 200) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 2:
            if (t == 60) {
                m_Anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 70) {
                // bullet_fan_aimed(RingBall, Red, 7, 1, 1.4, 0, 0, 0.628, 0x3) — Normal: 7 bullets
                glm::vec2 shootPos = enemy.m_Pos + glm::vec2{12.0f, -12.0f};
                m_BulletManager.SpawnFanAimed(shootPos, m_PlayerPos, EBulletType::RingBall,
                                              EBulletColor::Red, 7, 1.4f, 0.0f, 0.628f);
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = dir * 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 3:
            if (t == 60) {
                m_Anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = dir * 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;
    }
}

// ── Physics ──────────────────────────────────────────────────────────────────

void EnemyManager::UpdatePhysics(Enemy& enemy) {
    enemy.m_Pos.x += std::cos(enemy.m_Angle) * enemy.m_Speed;
    enemy.m_Pos.y += std::sin(enemy.m_Angle) * enemy.m_Speed;
    enemy.m_Angle += enemy.m_AngularVelocity;
    enemy.m_Speed += enemy.m_Acceleration;
}

// ── Timeline ─────────────────────────────────────────────────────────────────

void EnemyManager::SetTimeline(const TimelineEntry* entries, int count) {
    m_Timeline     = entries;
    m_TimelineSize = count;
    m_TimelineIdx  = 0;
}

void EnemyManager::RunTimeline() {
    if (!m_Timeline) return;
    while (m_TimelineIdx < m_TimelineSize && m_Timeline[m_TimelineIdx].frame <= m_Frame) {
        const auto& e = m_Timeline[m_TimelineIdx];
        if (e.frame == m_Frame) {
            SpawnEnemy(e.subId, e.x, e.y, e.life, e.score, e.mirrored);
        }
        m_TimelineIdx++;
    }
}

// ── Main update ──────────────────────────────────────────────────────────────

void EnemyManager::Update(const glm::vec2& playerPos, GameManager& gm) {
    m_PlayerPos = playerPos;
    RunTimeline();

    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;

        RunSub(enemy);

        // RunSub may have killed the enemy (e.g. enemy_delete)
        if (!enemy.m_Alive) {
            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
            continue;
        }

        UpdatePhysics(enemy);

        // Sync position to ANM VM and execute animation
        enemy.m_Vm.pos = enemy.m_Pos;
        m_Anm.UpdateObjects(enemy.m_Vm);

        enemy.m_FrameTimer++;

        // Despawn if far off-screen (generous bounds)
        if (!Util::IsInGameBounds(enemy.m_Pos.x, enemy.m_Pos.y, 0, 0, -200, -200, 600, 700)) {
            enemy.m_Alive = false;
            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
        }
    }

    m_Renderer.Update();
    m_BulletManager.Update();
    m_ItemManager.Update(playerPos, gm);
    m_Frame++;
}

// deal damage to enemies based on player's bullets, return total score gained from kills
int EnemyManager::ApplyPlayerBulletDamage(Player& player) {
    int totalScore = 0;
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;

        int dmg = player.CalcDamageToEnemy(enemy.m_Pos, enemy.m_HitboxSize);

        if (dmg <= 0) continue;
        enemy.m_Life -= dmg;

        if (enemy.m_Life <= 0) {
            enemy.m_Alive = false;
            totalScore += enemy.m_Score;

            if (enemy.m_ItemDrop >= 0) {
                m_ItemManager.SpawnItem(enemy.m_Pos, static_cast<ItemType>(enemy.m_ItemDrop));
            } else {
                if (m_RandomItemSpawnIndex++ % 3 == 0) {
                    m_ItemManager.SpawnItem(enemy.m_Pos, RANDOM_ITEM_TABLE[m_RandomItemTableIndex]);
                    m_RandomItemTableIndex = (m_RandomItemTableIndex + 1) % 32;
                }
            }

            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
        }
    }
    return totalScore;
}

bool EnemyManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    return m_BulletManager.CheckPlayerHit(playerPos, playerHitboxSize);
}

void EnemyManager::ClearAllBullets() {
    m_BulletManager.ClearAll();
}
