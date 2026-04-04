#include "Enemy/EnemyManager.hpp"

#include <cmath>

#include "Anm/AnmDefs.hpp"
#include "Util/Math.hpp"

// ── Stage 1 timeline (Normal difficulty) ─────────────────────────────────────
// Transcribed from ecldata1.txt timeline0
// Format: {frame, subId, x, y, life, score, mirrored}
const TimelineEntry STAGE1_TIMELINE[] = {
    // Wave 1: sub0 from left
    {128, 0, 60.0f, -32.0f, 8, 300, false},
    {144, 0, 68.0f, -32.0f, 32, 300, false},
    {160, 0, 76.0f, -32.0f, 32, 300, false},
    {176, 0, 84.0f, -32.0f, 32, 300, false},
    {192, 0, 92.0f, -32.0f, 32, 300, false},

    // Wave 1: sub0 from right (mirror)
    {256, 0, 324.0f, -32.0f, 32, 300, true},
    {272, 0, 316.0f, -32.0f, 32, 300, true},
    {288, 0, 308.0f, -32.0f, 32, 300, true},
    {304, 0, 300.0f, -32.0f, 32, 300, true},
    {320, 0, 292.0f, -32.0f, 32, 300, true},
    {336, 0, 284.0f, -32.0f, 32, 300, true},
    {352, 0, 276.0f, -32.0f, 32, 300, true},
    {368, 0, 268.0f, -32.0f, 32, 300, true},

    // Wave 2: sub1 left + right pairs
    {432, 1, 32.0f, -32.0f, 32, 300, false},
    {432, 1, 352.0f, -32.0f, 32, 300, true},
    {448, 1, 48.0f, -32.0f, 32, 300, false},
    {448, 1, 336.0f, -32.0f, 32, 300, true},
    {464, 1, 64.0f, -32.0f, 32, 300, false},
    {464, 1, 320.0f, -32.0f, 32, 300, true},
    {480, 1, 80.0f, -32.0f, 32, 300, false},
    {480, 1, 304.0f, -32.0f, 32, 300, true},
    {496, 1, 96.0f, -32.0f, 32, 300, false},
    {496, 1, 288.0f, -32.0f, 32, 300, true},
    {512, 1, 112.0f, -32.0f, 32, 300, false},
    {512, 1, 272.0f, -32.0f, 32, 300, true},
    {528, 1, 128.0f, -32.0f, 32, 300, false},
    {528, 1, 256.0f, -32.0f, 32, 300, true},
    {544, 1, 144.0f, -32.0f, 32, 300, false},
    {544, 1, 240.0f, -32.0f, 32, 300, true},
    {560, 1, 160.0f, -32.0f, 32, 300, false},
    {560, 1, 224.0f, -32.0f, 32, 300, true},
    {576, 1, 176.0f, -32.0f, 32, 300, false},
    {576, 1, 208.0f, -32.0f, 8, 300, true},

    // Wave 3: sub2/sub3 scattered
    {640, 2, 32.0f, -32.0f, 80, 700, false},
    {700, 3, 256.0f, -32.0f, 80, 700, true},
    {750, 3, 128.0f, -32.0f, 80, 700, false},
    {800, 2, 352.0f, -32.0f, 80, 700, true},
    {850, 3, 24.0f, -32.0f, 80, 700, false},
    {890, 3, 304.0f, -32.0f, 80, 700, true},
    {930, 2, 144.0f, -32.0f, 80, 700, false},
    {960, 3, 344.0f, -32.0f, 80, 700, true},
    {990, 3, 32.0f, -32.0f, 80, 700, false},
    {1020, 2, 240.0f, -32.0f, 80, 700, true},
    {1040, 3, 24.0f, -32.0f, 80, 700, false},
    {1060, 3, 304.0f, -32.0f, 80, 700, true},
    {1080, 2, 144.0f, -32.0f, 80, 700, false},
    {1090, 3, 344.0f, -32.0f, 80, 700, true},
    {1100, 2, 32.0f, -32.0f, 80, 700, false},
    {1110, 2, 240.0f, -32.0f, 80, 700, true},
};
constexpr int TIMELINE_SIZE = sizeof(STAGE1_TIMELINE) / sizeof(STAGE1_TIMELINE[0]);

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
        enemy.m_Pos        = {x, y};
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
            break;

        case 1:  // Small fairy variant
            m_Anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            break;

        case 2:  // Medium fairy: stops and shoots
            m_Anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            break;

        case 3:  // Medium fairy: stops (no bullet on Normal)
            m_Anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
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

void EnemyManager::RunTimeline() {
    while (m_TimelineIdx < TIMELINE_SIZE && STAGE1_TIMELINE[m_TimelineIdx].frame <= m_Frame) {
        const auto& e = STAGE1_TIMELINE[m_TimelineIdx];
        if (e.frame == m_Frame) {
            SpawnEnemy(e.subId, e.x, e.y, e.life, e.score, e.mirrored);
        }
        m_TimelineIdx++;
    }
}

// ── Main update ──────────────────────────────────────────────────────────────

void EnemyManager::Update(const glm::vec2& playerPos) {
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
    m_Frame++;
}
