#include "Enemy/EnemyManager.hpp"

#include <cmath>
#include <cstdlib>

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
    m_Anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    m_Anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

// ── Helpers ──────────────────────────────────────────────────────────────────

void EnemyManager::TransitionToSub(Enemy& enemy, int newSub) {
    enemy.m_SubId      = newSub;
    enemy.m_FrameTimer = -1;  // incremented to 0 at end of Update frame
}

void EnemyManager::MoveRandInBounds(Enemy& enemy) {
    float angle =
        ((float)rand() / RAND_MAX) * 2.0f * (Util::HALF_PI * 2.0f) - (Util::HALF_PI * 2.0f);

    // Push angle away from bounds edges (margin = 25% of bounds size)
    float mx = (enemy.m_BoundsMax.x - enemy.m_BoundsMin.x) * 0.25f;
    float my = (enemy.m_BoundsMax.y - enemy.m_BoundsMin.y) * 0.25f;

    if (enemy.m_Pos.x < enemy.m_BoundsMin.x + mx && std::cos(angle) < 0)
        angle = std::atan2(std::sin(angle), -std::cos(angle));
    if (enemy.m_Pos.x > enemy.m_BoundsMax.x - mx && std::cos(angle) > 0)
        angle = std::atan2(std::sin(angle), -std::cos(angle));
    if (enemy.m_Pos.y < enemy.m_BoundsMin.y + my && std::sin(angle) < 0)
        angle = std::atan2(-std::sin(angle), std::cos(angle));
    if (enemy.m_Pos.y > enemy.m_BoundsMax.y - my && std::sin(angle) > 0)
        angle = std::atan2(-std::sin(angle), std::cos(angle));

    enemy.m_Angle = angle;
}

void EnemyManager::StartLerpTo(Enemy& enemy, float targetX, float targetY, int frames) {
    enemy.m_IsLerping   = true;
    enemy.m_LerpOrigin  = enemy.m_Pos;
    enemy.m_LerpTarget  = Util::GameFieldToScreen(targetX, targetY);
    enemy.m_LerpFrames  = frames;
    enemy.m_LerpElapsed = 0;
}

void EnemyManager::StartLerpDir(Enemy& enemy, float speed, int frames) {
    enemy.m_IsLerping  = true;
    enemy.m_LerpOrigin = enemy.m_Pos;
    enemy.m_LerpTarget = enemy.m_Pos + glm::vec2{std::cos(enemy.m_Angle), std::sin(enemy.m_Angle)} *
                                           (speed * frames / 2.0f);
    enemy.m_LerpFrames  = frames;
    enemy.m_LerpElapsed = 0;
    enemy.m_Speed       = 0;
}

void EnemyManager::UpdateBossCallbacks(Enemy& enemy, GameManager& /*gm*/) {
    if (!enemy.m_IsBoss) return;

    enemy.m_BossTimer++;

    // Life callback: fires when life drops below threshold
    if (enemy.m_LifeCallbackThreshold >= 0 && enemy.m_Life < enemy.m_LifeCallbackThreshold) {
        enemy.m_Life                  = enemy.m_LifeCallbackThreshold;
        int sub                       = enemy.m_LifeCallbackSub;
        enemy.m_LifeCallbackThreshold = -1;
        enemy.m_LifeCallbackSub       = -1;
        enemy.m_TimerCallbackSub      = enemy.m_DeathCallbackSub;
        // Kill non-boss enemies
        for (auto& e : m_Enemies) {
            if (e.m_Alive && !e.m_IsBoss) e.m_Life = 0;
        }
        TransitionToSub(enemy, sub);
        return;
    }

    // Timer callback: fires when boss timer reaches threshold
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
        TransitionToSub(enemy, sub);
        return;
    }
}

// ── Spawning ─────────────────────────────────────────────────────────────────

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

        InitSub(enemy);

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

// ── Sub initialization (frame-0 actions) ─────────────────────────────────────

void EnemyManager::InitSub(Enemy& enemy) {
    int offset = Anm::STG1ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
            m_Anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 1:
            m_Anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 2:
            m_Anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;
            break;

        case 3:
            m_Anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;
            break;

        case 8: {
            m_Anm.SetScript(enemy.m_Vm, offset + 128, offset);
            enemy.m_HitboxSize             = {48, 56};
            enemy.m_ItemDrop               = -1;
            enemy.m_ItemDropCount          = 0;
            enemy.m_Pos                    = Util::GameFieldToScreen(192.0f, -32.0f);
            enemy.m_IsBoss                 = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_BossMaxLife            = 6000;
            enemy.m_DeathCallbackSub       = 6;
            enemy.m_LifeCallbackThreshold  = -1;  // {"HL"} only — Normal has no life callback
            enemy.m_LifeCallbackSub        = -1;
            enemy.m_TimerCallbackThreshold = 1440;
            enemy.m_TimerCallbackSub       = 7;
            enemy.m_BlocksTimeline         = true;
            break;
        }

        case 10: {
            int off2 = Anm::STG1ENM2.offset;
            m_Anm.SetScript(enemy.m_Vm, off2 + 128, off2);
            enemy.m_HitboxSize    = {48, 56};
            enemy.m_ItemDrop      = -1;
            enemy.m_ItemDropCount = 0;
            // Override to entry position (game field coords 320, -32)
            enemy.m_Pos              = Util::GameFieldToScreen(320.0f, -32.0f);
            enemy.m_IsBoss           = true;
            enemy.m_CanTakeDamage    = false;
            enemy.m_DeathCallbackSub = 16;
            break;
        }

        default:
            break;
    }
}

// ── Sub execution ─────────────────────────────────────────────────────────────

void EnemyManager::RunSub(Enemy& enemy) {
    int   t      = enemy.m_FrameTimer;
    int   offset = Anm::STG1ENM.offset;
    float dir    = enemy.m_Mirrored ? -1.0f : 1.0f;

    switch (enemy.m_SubId) {
        // ── Regular enemies ───────────────────────────────────────────────────
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

        // ── Mid-boss: escape (timer callback) ────────────────────────────────
        case 7: {
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                m_BulletManager.ClearAll();
                m_LaserManager.ClearAll();
                enemy.m_Speed = 0.0f;
                StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 60) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    m_Renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }

        // ── Mid-boss: death (life depleted) ──────────────────────────────────
        case 6: {
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                m_BulletManager.ClearAll();
                m_LaserManager.ClearAll();
                enemy.m_Speed = 0.0f;
                for (int k = 0; k < 5; k++)
                    m_ItemManager.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
                StartLerpTo(enemy, 192.0f, -64.0f, 120);
            }
            if (t == 120) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    m_Renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }

        // ── Mid-boss: main pattern ────────────────────────────────────────────
        case 8: {
            // 3 concentric aimed circles: speeds 2.0 / 3.5 / 5.0 (Normal = 3 stacks)
            auto circle3 = [&](EBulletColor c) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall, c,
                                                 16, 2.0f);
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall, c,
                                                 16, 3.5f);
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall, c,
                                                 16, 5.0f);
            };

            if (t == 0) StartLerpTo(enemy, 320.0f, 128.0f, 60);
            if (t == 60) enemy.m_CanTakeDamage = true;
            if (t == 160) circle3(EBulletColor::Blue);
            // Loop 1
            if (t == 202) StartLerpTo(enemy, 192.0f, 64.0f, 60);
            if (t == 384) StartLerpTo(enemy, 64.0f, 96.0f, 60);
            if (t == 414) circle3(EBulletColor::Green);
            if (t == 444) circle3(EBulletColor::Yellow);
            if (t == 526) StartLerpTo(enemy, 192.0f, 80.0f, 60);
            if (t == 708) StartLerpTo(enemy, 320.0f, 96.0f, 60);
            if (t == 738) circle3(EBulletColor::Blue);
            if (t == 768) circle3(EBulletColor::Red);
            // Loop 2 (+648 from loop 1 start)
            if (t == 850) StartLerpTo(enemy, 192.0f, 64.0f, 60);
            if (t == 1032) StartLerpTo(enemy, 64.0f, 96.0f, 60);
            if (t == 1062) circle3(EBulletColor::Green);
            if (t == 1092) circle3(EBulletColor::Yellow);
            if (t == 1174) StartLerpTo(enemy, 192.0f, 80.0f, 60);
            if (t == 1356) StartLerpTo(enemy, 320.0f, 96.0f, 60);
            if (t == 1386) circle3(EBulletColor::Blue);
            if (t == 1416) circle3(EBulletColor::Red);
            // timer_callback at 1440 → sub7 (handled by UpdateBossCallbacks)
            break;
        }

        // ── Mid-boss: Moonlight Ray spell (life callback at 500 HP) ──────────
        // Compromise: laser rotation uses fixed angularVelocity (no per-frame control).
        case 9: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 1320;
                enemy.m_TimerCallbackSub       = 6;
                StartLerpTo(enemy, 192.0f, 96.0f, 120);
            }
            if (t == 120) enemy.m_CanTakeDamage = true;
            {
                int loopT = (t >= 120) ? (t - 120) % 330 : -1;
                if (loopT == 0) {
                    // 2 counter-rotating lasers: angles 22.5° and 157.5°
                    m_LaserManager.SpawnAtAngle(enemy.m_Pos, 0.3926991f, 500.0f, 32.0f, 30, 120, 30,
                                                16, 120, 0.008267349f);
                    m_LaserManager.SpawnAtAngle(enemy.m_Pos, 2.7488935f, 500.0f, 32.0f, 30, 120, 30,
                                                16, 120, -0.008267349f);
                }
                if (loopT == 210) {
                    MoveRandInBounds(enemy);
                    StartLerpDir(enemy, 2.0f, 120);
                }
            }
            break;
        }

        // ── Boss: entry ───────────────────────────────────────────────────────
        case 10: {
            if (t == 0) {
                // Decelerate-lerp from current pos to (192, 96) in 60 frames
                StartLerpTo(enemy, 192.0f, 96.0f, 60);
            }
            if (t == 65) {
                int off2 = Anm::STG1ENM2.offset;
                m_Anm.SetScript(enemy.m_Vm, off2 + 133, off2);
                TransitionToSub(enemy, 11);
            }
            break;
        }

        // ── Boss: Phase 1 non-spell ───────────────────────────────────────────
        case 11: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = true;
                enemy.m_Life                   = 7000;
                enemy.m_BossMaxLife            = 7000;
                enemy.m_BossTimer              = 0;
                enemy.m_LifeCallbackThreshold  = 900;
                enemy.m_LifeCallbackSub        = 22;
                enemy.m_TimerCallbackThreshold = 2100;
                enemy.m_TimerCallbackSub       = 22;
                enemy.m_DeathCallbackSub       = 16;
            }
            if (t == 100) TransitionToSub(enemy, 12);
            break;
        }

        // ── Boss: Phase 1 attack – Ball fan stack (random cycle) ──────────────
        case 12: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
            }
            // 7 stacked fan bursts at t=12,20,28,36,44,52,60 (every 8f starting at 12)
            // Normal: t=12 baseSpeed=3.0, t=20-60 baseSpeed=4.0, stacks=10, inc=1.0
            if (t >= 12 && t <= 60 && (t - 12) % 8 == 0) {
                float base = (t == 12) ? 3.0f : 4.0f;
                m_BulletManager.SpawnFanStack(
                    enemy.m_Pos, m_PlayerPos, EBulletType::Ball,
                    (((t - 12) / 8) % 2 == 0) ? EBulletColor::Red : EBulletColor::DarkRed, 1, 10,
                    base, 1.0f, 0.0f, 0.09817477f);
            }
            if (t == 180) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 13 : (r == 1 ? 14 : 15));
            }
            break;
        }

        // ── Boss: Phase 1 attack – RingBall circles ───────────────────────────
        case 13: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 60) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                                 EBulletColor::Blue, 6, 4.0f);
            }
            if (t == 68) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Pellet,
                                                 EBulletColor::DarkBlue, 8, 3.0f);
            }
            if (t == 76) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                                 EBulletColor::Blue, 6, 2.0f);
            }
            if (t == 84) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Pellet,
                                                 EBulletColor::DarkBlue, 8, 3.0f);
            }
            if (t == 92) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                                 EBulletColor::Blue, 6, 4.0f);
            }
            if (t == 100) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Pellet,
                                                 EBulletColor::DarkBlue, 8, 3.0f);
            }
            if (t == 108) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                                 EBulletColor::Blue, 6, 2.0f);
            }
            if (t == 228) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 12 : (r == 1 ? 14 : 15));
            }
            break;
        }

        // ── Boss: Phase 1 attack – Rice fan + RingBall circle ─────────────────
        case 14: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 80) {
                // Normal: 2 ways, 16 count per way, speed 5.0, spread 0.06544985
                m_BulletManager.SpawnFanAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Rice,
                                              EBulletColor::Red, 2 * 16, 5.0f, 0.0f, 0.06544985f);
            }
            if (t == 110) {
                // Normal: 16 bullets, speed 2.0
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                                 EBulletColor::Blue, 16, 2.0f);
            }
            if (t == 200) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 13 : (r == 1 ? 12 : 15));
            }
            break;
        }

        // ── Boss: Phase 1 attack – RingBall spiral ────────────────────────────
        case 15: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
                // Randomise spiral direction
                enemy.m_Mirrored = (rand() % 2) == 0;
            }
            // 16 bullets fired every 2 frames, speed 1.0..4.75, offset rotates
            if (t >= 0 && t < 32 && t % 2 == 0) {
                int   step   = t / 2;
                float speed  = 1.0f + step * 0.25f;
                float offset = enemy.m_Mirrored ? -(0.2617994f + step * 0.06544985f)
                                                : (0.2617994f + step * 0.06544985f);
                m_BulletManager.SpawnFanAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                              EBulletColor::Green, 1, speed, offset, 0.0f);
            }
            if (t == 124) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 13 : (r == 1 ? 14 : 12));
            }
            break;
        }

        // ── Boss: Phase 2 entry (after Night Bird or non-spell ends) ──────────
        case 16: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_Life                   = 7500;
                enemy.m_BossMaxLife            = 7500;
                enemy.m_BossTimer              = 0;
                enemy.m_LifeCallbackThreshold  = 800;
                enemy.m_LifeCallbackSub        = 23;
                enemy.m_TimerCallbackThreshold = 1800;
                enemy.m_TimerCallbackSub       = 23;
                enemy.m_DeathCallbackSub       = 17;
                // Drop 5 PowerSmall items
                for (int k = 0; k < 5; k++)
                    m_ItemManager.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
            }
            if (t == 200) {
                enemy.m_CanTakeDamage = true;
                TransitionToSub(enemy, 18);
            }
            break;
        }

        // ── Boss: death sequence ──────────────────────────────────────────────
        // Compromise: simplified — no particle effects, just mark dead.
        case 17: {
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                m_BulletManager.ClearAll();
                m_LaserManager.ClearAll();
            }
            if (t == 60) {
                enemy.m_Alive = false;
            }
            break;
        }

        // ── Boss: Phase 2 attack – RingBall fan + lasers (random cycle) ───────
        case 18: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 12) {
                // Normal: 1 way, speed 3.0
                m_BulletManager.SpawnFanAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                              EBulletColor::Green, 1, 3.0f, 0.0f, 0.0f);
            }
            // 7 aimed lasers at t=20,28,36,44,52,60,68 (every 8f)
            // Params from ECL: length=500, maxWidth=16, startTime=120, duration=60,
            //                  endTime=14, hitboxStart=16, hitboxEnd=120
            if (t >= 20 && t <= 60 && (t - 20) % 8 == 0) {
                m_LaserManager.SpawnAimed(enemy.m_Pos, m_PlayerPos, 500.0f, 16.0f, 120, 60, 14, 16,
                                          120);
            }
            if (t == 224) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 19 : (r == 1 ? 20 : 21));
            }
            break;
        }

        // ── Boss: Phase 2 attack – Rice circles ───────────────────────────────
        case 19: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 60) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Rice,
                                                 EBulletColor::Green, 24, 2.0f);
            }
            if (t == 90) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Pellet,
                                                 EBulletColor::Green, 28, 2.6f);
            }
            if (t == 120) {
                m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos, EBulletType::Rice,
                                                 EBulletColor::Green, 24, 2.0f);
            }
            if (t == 240) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 18 : (r == 1 ? 20 : 21));
            }
            break;
        }

        // ── Boss: Phase 2 attack – Rice fan x3 ───────────────────────────────
        case 20: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
            }
            // Normal: 4 ways, 2 rings, speed 3.0, spread 0.5235988
            if (t == 60)
                m_BulletManager.SpawnFanStack(enemy.m_Pos, m_PlayerPos, EBulletType::Rice,
                                              EBulletColor::Yellow, 4, 2, 3.0f, 0.0f, 0.0f,
                                              0.5235988f);
            if (t == 80)
                m_BulletManager.SpawnFanStack(enemy.m_Pos, m_PlayerPos, EBulletType::Rice,
                                              EBulletColor::Yellow, 5, 2, 3.0f, 0.0f, 0.0f,
                                              0.5235988f);
            if (t == 100)
                m_BulletManager.SpawnFanStack(enemy.m_Pos, m_PlayerPos, EBulletType::Rice,
                                              EBulletColor::Yellow, 7, 2, 3.0f, 0.0f, 0.0f,
                                              0.5235988f);
            if (t == 220) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 19 : (r == 1 ? 18 : 21));
            }
            break;
        }

        // ── Boss: Phase 2 attack – RingBall spiral (bigger) ──────────────────
        case 21: {
            if (t == 0) {
                MoveRandInBounds(enemy);
                StartLerpDir(enemy, 3.0f, 60);
                enemy.m_Mirrored = (rand() % 2) == 0;
            }
            // 16 bullets every 2 frames, 2 ways, speed 1.0..4.75, rotating offset
            if (t >= 0 && t < 32 && t % 2 == 0) {
                int   step   = t / 2;
                float speed  = 1.0f + step * 0.25f;
                float offset = enemy.m_Mirrored ? -(0.7139983f + step * 0.14279966f)
                                                : (0.7139983f + step * 0.14279966f);
                m_BulletManager.SpawnFanAimed(enemy.m_Pos, m_PlayerPos, EBulletType::RingBall,
                                              EBulletColor::Green, 2, speed, offset, 0.14279966f);
            }
            if (t == 124) {
                int r = rand() % 3;
                TransitionToSub(enemy, r == 0 ? 19 : (r == 1 ? 20 : 18));
            }
            break;
        }

        // ── Boss: Spell – Night Bird ──────────────────────────────────────────
        // Compromise: timer_callback_threshold(1500) reuses death_callback_sub (sub16→sub23→sub17).
        case 22: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 1500;
                // timerCallbackSub was already set to deathCallbackSub (sub16) by callback logic
                StartLerpTo(enemy, 192.0f, 96.0f, 120);
            }
            if (t == 120) {
                enemy.m_CanTakeDamage = true;
            }
            // Loop: 4 groups of 16 RingBall spiral bullets, then move
            // Each full cycle ≈ 228 frames (4*2*16=128f firing + 100f move)
            {
                int loopT = (t >= 120) ? (t - 120) % 228 : -1;
                if (loopT >= 0) {
                    // 4 groups of 16 bullets at 2-frame intervals
                    // Group 0 (DarkBlue, negative spiral): loopT 0..30
                    // Group 1 (DarkCyan, positive spiral): loopT 32..62
                    // Group 2 (Blue, negative spiral):     loopT 64..94
                    // Group 3 (Cyan, positive spiral):     loopT 96..126
                    // Move: loopT == 128
                    auto shootSpiral = [&](int groupStart, EBulletColor color, bool negDir) {
                        if (loopT >= groupStart && loopT < groupStart + 32 &&
                            (loopT - groupStart) % 2 == 0) {
                            int   step   = (loopT - groupStart) / 2;
                            float speed  = 1.0f + step * 0.2f;
                            float offset = negDir ? -(0.57119864f + step * 0.19634955f)
                                                  : (0.57119864f + step * 0.19634955f);
                            m_BulletManager.SpawnFanAimed(enemy.m_Pos, m_PlayerPos,
                                                          EBulletType::RingBall, color, 1, speed,
                                                          offset, 0.06544985f);
                        }
                    };
                    shootSpiral(0, EBulletColor::DarkBlue, true);
                    shootSpiral(32, EBulletColor::DarkCyan, false);
                    shootSpiral(64, EBulletColor::Blue, true);
                    shootSpiral(96, EBulletColor::Cyan, false);
                    if (loopT == 128) {
                        MoveRandInBounds(enemy);
                        StartLerpDir(enemy, 2.0f, 120);
                    }
                }
            }
            break;
        }

        // ── Boss: Spell – Demarcation ─────────────────────────────────────────
        // Compromise: bullet_effects (curved trajectory) not implemented.
        case 23: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 1500;
                StartLerpTo(enemy, 192.0f, 96.0f, 120);
            }
            if (t == 120) {
                enemy.m_CanTakeDamage = true;
            }
            // Loop: 3 Rice circle pairs every 60f, then move at 300f
            {
                int loopT = (t >= 120) ? (t - 120) % 360 : -1;
                if (loopT >= 0) {
                    // Normal: 12 bullets, speed 3.0
                    if (loopT == 0) {
                        m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos,
                                                         EBulletType::Rice, EBulletColor::Blue, 12,
                                                         3.0f);
                        m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos,
                                                         EBulletType::Rice, EBulletColor::Blue, 12,
                                                         3.0f);
                    }
                    if (loopT == 60) {
                        m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos,
                                                         EBulletType::Rice, EBulletColor::Green, 12,
                                                         3.0f);
                        m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos,
                                                         EBulletType::Rice, EBulletColor::Green, 12,
                                                         3.0f);
                    }
                    if (loopT == 120) {
                        m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos,
                                                         EBulletType::Rice, EBulletColor::Red, 12,
                                                         3.0f);
                        m_BulletManager.SpawnCircleAimed(enemy.m_Pos, m_PlayerPos,
                                                         EBulletType::Rice, EBulletColor::Red, 12,
                                                         3.0f);
                    }
                    if (loopT == 300) {
                        MoveRandInBounds(enemy);
                        StartLerpDir(enemy, 2.0f, 120);
                    }
                }
            }
            break;
        }

        default:
            break;
    }
}

// ── Physics ──────────────────────────────────────────────────────────────────

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

// ── Timeline ─────────────────────────────────────────────────────────────────

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

// ── Main update ──────────────────────────────────────────────────────────────

void EnemyManager::Update(const glm::vec2& playerPos, GameManager& gm) {
    m_PlayerPos = playerPos;
    RunTimeline();

    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;

        UpdateBossCallbacks(enemy, gm);
        if (!enemy.m_Alive) continue;

        RunSub(enemy);

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
    m_ItemManager.Update(playerPos, gm);
    m_Frame++;
}

// ── Damage ────────────────────────────────────────────────────────────────────

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
                // Boss: trigger death callback instead of dying outright
                enemy.m_Life             = 0;
                enemy.m_CanTakeDamage    = false;
                int sub                  = enemy.m_DeathCallbackSub;
                enemy.m_DeathCallbackSub = -1;
                if (sub >= 0) {
                    m_BulletManager.ClearAll();
                    m_LaserManager.ClearAll();
                    TransitionToSub(enemy, sub);
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
                        m_ItemManager.SpawnItem(enemy.m_Pos,
                                                static_cast<ItemType>(enemy.m_ItemDrop));
                    } else {
                        if (m_RandomItemSpawnIndex++ % 3 == 0) {
                            m_ItemManager.SpawnItem(enemy.m_Pos,
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
