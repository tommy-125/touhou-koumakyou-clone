#include "Scene/Stage1/Stage1Script.hpp"

#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace {

void StartRandomAttackMove(Enemy& enemy, const EnemySubCtx& ctx, float speed = 3.0f, int frames = 60) {
    ctx.MoveRandInBounds(enemy);
    ctx.StartLerpDir(enemy, speed, frames);
}

void TransitionToRandomSub(Enemy& enemy, const EnemySubCtx& ctx, int subA, int subB, int subC) {
    const int roll = rand() % 3;
    ctx.TransitionToSub(enemy, roll == 0 ? subA : (roll == 1 ? subB : subC));
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int phaseIndex,
                     int spellBonus, int timerFrames) {
    enemy.m_CanTakeDamage          = false;
    enemy.m_InSpellcard            = true;
    enemy.m_ShowSpellName          = true;
    enemy.m_BossTitle              = title;
    enemy.m_BossPhaseIndex         = phaseIndex;
    enemy.m_SpellcardBonus         = spellBonus;
    enemy.m_BossTimer              = 0;
    enemy.m_TimerCallbackThreshold = timerFrames;
    ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
}

}  // namespace

void Stage1Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

void Stage1Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    int offset = Anm::STG1ENM.offset;

    switch (enemy.m_SubId) {
        // ══════════════════════════════════════════════════════════════════════
        // Regular enemies (small/medium fairies)
        // ══════════════════════════════════════════════════════════════════════
        case 0:  // Small fairy — straight-down with angular drift
        case 1:  // Small fairy — single-turn variant
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 2:  // Medium fairy — stops and shoots fan
        case 3:  // Medium fairy — same movement, no shot on Normal
            ctx.anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;
            break;

        // ══════════════════════════════════════════════════════════════════════
        // Mid-boss (Rumia — stg1enm sprite 128, short preview encounter)
        // ══════════════════════════════════════════════════════════════════════
        case 8: {  // Mid-boss main pattern entry
            ctx.anm.SetScript(enemy.m_Vm, offset + 128, offset);
            enemy.m_HitboxSize             = {48, 56};
            enemy.m_ItemDrop               = -1;
            enemy.m_ItemDropCount          = 0;
            enemy.m_Pos                    = Util::GameFieldToScreen(192.0f, -32.0f);
            enemy.m_IsBoss                 = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_BossMaxLife            = 6000;
            enemy.m_DeathCallbackSub       = 6;
            enemy.m_LifeCallbackThreshold  = -1;  // Normal has no mid-boss life callback
            enemy.m_LifeCallbackSub        = -1;
            enemy.m_TimerCallbackThreshold = 1440;
            enemy.m_TimerCallbackSub       = 7;
            enemy.m_BlocksTimeline         = true;
            enemy.m_BossTitle              = "Rumia";
            enemy.m_BossPhaseIndex         = 0;
            break;
        }

        // ══════════════════════════════════════════════════════════════════════
        // Boss (Rumia — stg1enm2 sprite 128)
        // ══════════════════════════════════════════════════════════════════════
        case 10: {  // Boss entry
            int off2 = Anm::STG1ENM2.offset;
            ctx.anm.SetScript(enemy.m_Vm, off2 + 128, off2);
            enemy.m_HitboxSize       = {48, 56};
            enemy.m_ItemDrop         = -1;
            enemy.m_ItemDropCount    = 0;
            enemy.m_Pos              = Util::GameFieldToScreen(320.0f, -32.0f);
            enemy.m_IsBoss           = true;
            enemy.m_CanTakeDamage    = false;
            enemy.m_DeathCallbackSub = 16;
            enemy.m_BossTitle        = "Rumia";
            break;
        }

        default:
            break;
    }
}

void Stage1Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    int   t      = enemy.m_FrameTimer;
    int   offset = Anm::STG1ENM.offset;
    float dir    = enemy.m_Mirrored ? -1.0f : 1.0f;

    switch (enemy.m_SubId) {
        // ══════════════════════════════════════════════════════════════════════
        // Regular enemies (small/medium fairies) — Stage waves 1–3
        // ══════════════════════════════════════════════════════════════════════
        case 0:  // Small fairy — angular drift pattern A
            if (t == 40) enemy.m_AngularVelocity = dir * -0.024543693f;
            if (t == 120) enemy.m_AngularVelocity = dir * 0.019634955f;
            if (t == 220) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 1:  // Small fairy — angular drift pattern B
            if (t == 100) enemy.m_AngularVelocity = dir * 0.019634955f;
            if (t == 200) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 2:  // Medium fairy — stops, shoots 7-way fan, re-accelerates
            if (t == 60) {
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 70) {
                glm::vec2 shootPos = enemy.m_Pos + glm::vec2{12.0f, -12.0f};
                ctx.bullets.SpawnFanAimed(shootPos, ctx.playerPos, EBulletType::RingBall,
                                          EBulletColor::Red, 7, 1.4f, 0.0f, 0.62831855f, true);
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = dir * 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 3:  // Medium fairy — same movement, no shot on Normal
            if (t == 60) {
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = dir * 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        // ══════════════════════════════════════════════════════════════════════
        // Mid-boss (Rumia — preview encounter)
        // ══════════════════════════════════════════════════════════════════════
        case 8: {  // Mid-boss main pattern (Sub4 scatter waves + Sub5 random + circle3 rings)
            auto circle3 = [&](EBulletColor c) {
                constexpr float s1 = 2.0f, s2 = 1.5f;
                constexpr int   n = 3;
                for (int ring = 0; ring < n; ring++) {
                    float spd = s1 - (s1 - s2) * ring / n;
                    ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                                 c, 16, spd, true);
                }
            };

            static constexpr EBulletColor kSub4Colors[5] = {
                EBulletColor::Blue, EBulletColor::Red, EBulletColor::Green, EBulletColor::Yellow,
                EBulletColor::Orange};
            static constexpr float kSub4Offsets[5] = {0.0f, 0.06544985f, 0.1308997f, 0.19634955f,
                                                      0.2617994f};
            auto                   sub4Wave        = [&](int wave) {
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Pellet, kSub4Colors[wave], 8,
                                        0.0f, kSub4Offsets[wave], true, 0.02f);
            };

            auto randSpeed = [&]() { return (rand() % 1000) / 1000.0f * 3.0f + 0.5f; };
            auto randAngle = [&]() {
                return ((rand() % 1000) / 1000.0f * 2.0f - 1.0f) * 3.14159265f;
            };
            auto sub5Wave = [&](int wave) {
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Pellet, kSub4Colors[wave], 4,
                                        randSpeed(), randAngle(), true, 0.7f);
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Rice, kSub4Colors[wave], 4,
                                        randSpeed(), randAngle(), true, 0.7f);
            };

            if (t == 0) ctx.StartLerpTo(enemy, 320.0f, 128.0f, 60);
            if (t == 60) enemy.m_CanTakeDamage = true;
            if (t == 160) circle3(EBulletColor::Blue);
            for (int cycle = 0; cycle < 2; cycle++) {
                const int base = 192 + cycle * 648;
                if (t == base + 10) ctx.StartLerpTo(enemy, 192.0f, 64.0f, 60);
                for (int w = 0; w < 5; w++) {
                    if (t == base + 70 + w * 8) sub4Wave(w);
                }
                if (t == base + 192) ctx.StartLerpTo(enemy, 64.0f, 96.0f, 60);
                if (t == base + 222) circle3(EBulletColor::Green);
                if (t == base + 252) circle3(EBulletColor::Yellow);
                if (t == base + 334) ctx.StartLerpTo(enemy, 192.0f, 80.0f, 60);
                for (int w = 0; w < 5; w++) {
                    if (t == base + 394 + w * 8) sub5Wave(w);
                }
                if (t == base + 516) ctx.StartLerpTo(enemy, 320.0f, 96.0f, 60);
                if (t == base + 546) circle3(EBulletColor::Blue);
                if (t == base + 576) circle3(EBulletColor::Red);
            }
            if (t == 850) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            if (t == 910) enemy.m_Alive = false;
            break;
        }

        case 7: {  // Mid-boss escape (timer callback from Sub8)
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Speed = 0.0f;
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 60) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    ctx.renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }

        case 6: {  // Mid-boss death (death callback from Sub8)
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Speed = 0.0f;
                for (int k = 0; k < 5; k++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
            }
            if (t == 40) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 120);
            if (t == 160) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    ctx.renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }

        // ══════════════════════════════════════════════════════════════════════
        // Boss (Rumia) — Phase 1
        // ══════════════════════════════════════════════════════════════════════
        case 10: {  // Boss entry — lerp in, swap sprite, go non-spell
            if (t == 0) {
                ctx.StartLerpTo(enemy, 192.0f, 96.0f, 60);
            }
            if (t == 65) {
                int off2 = Anm::STG1ENM2.offset;
                ctx.anm.SetScript(enemy.m_Vm, off2 + 133, off2);
                ctx.TransitionToSub(enemy, 11);
            }
            break;
        }

        case 11: {  // Phase 1 non-spell init (HP 7000, life→22 @900, timer→22 @2100)
            if (t == 0) {
                enemy.m_CanTakeDamage          = true;
                enemy.m_ShowSpellName          = false;
                enemy.m_BossTitle              = "Rumia";
                enemy.m_BossPhaseIndex         = 1;
                enemy.m_SpellcardBonus         = 0;
                enemy.m_Life                   = 7000;
                enemy.m_BossMaxLife            = 7000;
                enemy.m_BossTimer              = 0;
                enemy.m_LifeCallbackThreshold  = 900;
                enemy.m_LifeCallbackSub        = 22;
                enemy.m_TimerCallbackThreshold = 2100;
                enemy.m_TimerCallbackSub       = 22;
                enemy.m_DeathCallbackSub       = 16;
            }
            if (t == 100) ctx.TransitionToSub(enemy, 12);
            break;
        }

        case 12: {  // Phase 1 attack — Ball fan stack
            if (t == 0) StartRandomAttackMove(enemy, ctx);
            if (t >= 12 && t <= 60 && (t - 12) % 8 == 0) {
                const int burst = (t - 12) / 8;
                ctx.bullets.SpawnFanStack(
                    enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
                    (burst & 1) == 0 ? EBulletColor::Red : EBulletColor::DarkRed, 1, 10,
                    burst == 0 ? 3.0f : 4.0f, 1.0f, 0.0f, 0.09817477f);
            }
            if (t == 180) TransitionToRandomSub(enemy, ctx, 13, 14, 15);
            break;
        }

        case 13: {  // Phase 1 attack — RingBall + Pellet interleaved circles
            if (t == 0) StartRandomAttackMove(enemy, ctx);
            if (t == 60) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 12, 4.0f);
            }
            if (t == 68) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::DarkBlue, 16, 3.0f);
            }
            if (t == 76) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 12, 2.0f, -0.1308997f);
            }
            if (t == 84) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::DarkBlue, 16, 3.0f);
            }
            if (t == 92) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 12, 4.0f, 0.1308997f);
            }
            if (t == 100) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::DarkBlue, 16, 3.0f);
            }
            if (t == 108) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 12, 2.0f, -0.1308997f);
            }
            if (t == 228) TransitionToRandomSub(enemy, ctx, 12, 14, 15);
            break;
        }

        case 14: {  // Phase 1 attack — Rice fan stack + RingBall circle
            if (t == 0) StartRandomAttackMove(enemy, ctx);
            if (t == 80) {
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Red, 2, 16, 5.0f, 1.0f, 0.0f, 0.06544985f);
            }
            if (t == 110) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 16, 2.0f);
            }
            if (t == 200) TransitionToRandomSub(enemy, ctx, 13, 12, 15);
            break;
        }

        case 15: {  // Phase 1 attack — RingBall converging spiral
            if (t == 0) {
                StartRandomAttackMove(enemy, ctx);
                enemy.m_Mirrored = (rand() % 2) == 0;
            }
            if (t >= 0 && t < 32 && t % 2 == 0) {
                int   step   = t / 2;
                float speed  = 1.0f + step * 0.25f;
                float offset = enemy.m_Mirrored ? (-0.2617994f + step * 0.06544985f)
                                                : (0.2617994f - step * 0.06544985f);
                ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                          EBulletColor::Green, 1, speed, offset, 0.0f);
            }
            if (t == 124) TransitionToRandomSub(enemy, ctx, 13, 14, 12);
            break;
        }

        case 22: {  // Phase 1 spellcard — Night Bird (8-group RingBall spirals × 2 passes, then
                    // move)
            if (t == 0) StartSpellPhase(enemy, ctx, "Night Bird", 2, 2000000, 1500);
            if (t == 120) {
                enemy.m_CanTakeDamage = true;
            }
            {
                int loopT = (t >= 120) ? (t - 120) % 356 : -1;
                if (loopT >= 0) {
                    auto shootSpiral = [&](int groupStart, EBulletColor color, float initOffset,
                                           float angleStep) {
                        if (loopT >= groupStart && loopT < groupStart + 32 &&
                            (loopT - groupStart) % 2 == 0) {
                            int   step   = (loopT - groupStart) / 2;
                            float speed  = 1.0f + step * 0.2f;
                            float offset = initOffset + step * angleStep;
                            ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos,
                                                      EBulletType::RingBall, color, 1, speed,
                                                      offset, 0.06544985f);
                        }
                    };
                    shootSpiral(0, EBulletColor::DarkBlue, -0.57119864f, +0.19634955f);
                    shootSpiral(32, EBulletColor::DarkCyan, +0.57119864f, -0.19634955f);
                    shootSpiral(64, EBulletColor::Blue, -0.7853982f, +0.2617994f);
                    shootSpiral(96, EBulletColor::Cyan, +0.7853982f, -0.2617994f);
                    shootSpiral(128, EBulletColor::DarkBlue, -0.57119864f, +0.19634955f);
                    shootSpiral(160, EBulletColor::DarkCyan, +0.57119864f, -0.19634955f);
                    shootSpiral(192, EBulletColor::Blue, -0.7853982f, +0.2617994f);
                    shootSpiral(224, EBulletColor::Cyan, +0.7853982f, -0.2617994f);
                    if (loopT == 256) {
                        ctx.MoveRandInBounds(enemy);
                        ctx.StartLerpDir(enemy, 2.0f, 120);
                    }
                }
            }
            break;
        }

        // ══════════════════════════════════════════════════════════════════════
        // Boss (Rumia) — Phase 2
        // ══════════════════════════════════════════════════════════════════════
        case 16: {  // Phase 2 entry (HP 7500, life→23 @800, timer→23 @1800, death→17; drops 5
                    // items)
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_InSpellcard            = false;
                enemy.m_ShowSpellName          = false;
                enemy.m_BossTitle              = "Rumia";
                enemy.m_BossPhaseIndex         = 3;
                enemy.m_SpellcardBonus         = 0;
                enemy.m_Life                   = 7500;
                enemy.m_BossMaxLife            = 7500;
                enemy.m_BossTimer              = 0;
                enemy.m_LifeCallbackThreshold  = 800;
                enemy.m_LifeCallbackSub        = 23;
                enemy.m_TimerCallbackThreshold = 1800;
                enemy.m_TimerCallbackSub       = 23;
                enemy.m_DeathCallbackSub       = 17;
                for (int k = 0; k < 5; k++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
            }
            if (t == 200) {
                enemy.m_CanTakeDamage = true;
                ctx.TransitionToSub(enemy, 18);
            }
            break;
        }

        case 18: {  // Phase 2 attack — RingBall fan + 6 aimed lasers
            if (t == 0) StartRandomAttackMove(enemy, ctx);
            if (t == 12) {
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                          EBulletColor::Green, 1, 8, 3.0f, 1.0f, 0.0f, 0.09817477f);
            }
            if (t >= 20 && t <= 60 && (t - 20) % 8 == 0) {
                ctx.lasers.SpawnAimed(enemy.m_Pos, ctx.playerPos, 500.0f, 16.0f, 120, 60, 14, 16,
                                      120);
            }
            if (t == 224) TransitionToRandomSub(enemy, ctx, 19, 20, 21);
            break;
        }

        case 19: {  // Phase 2 attack — Rice + Pellet circles
            if (t == 0) StartRandomAttackMove(enemy, ctx);
            if (t == 60) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                             EBulletColor::Green, 24, 2.0f);
            }
            if (t == 90) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::Green, 28, 2.6f);
            }
            if (t == 120) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                             EBulletColor::Green, 24, 2.0f);
            }
            if (t == 240) TransitionToRandomSub(enemy, ctx, 18, 20, 21);
            break;
        }

        case 20: {  // Phase 2 attack — Rice fan stack × 3
            if (t == 0) StartRandomAttackMove(enemy, ctx);
            if (t == 60)
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Yellow, 4, 2, 3.0f, 1.0f, 0.0f, 0.5235988f);
            if (t == 80)
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Yellow, 5, 2, 3.0f, 1.0f, 0.0f, 0.5235988f);
            if (t == 100)
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Yellow, 7, 2, 3.0f, 1.0f, 0.0f,
                                          0.5235988f);
            if (t == 220) TransitionToRandomSub(enemy, ctx, 19, 18, 21);
            break;
        }

        case 21: {  // Phase 2 attack — RingBall wider converging spiral
            if (t == 0) {
                StartRandomAttackMove(enemy, ctx);
                enemy.m_Mirrored = (rand() % 2) == 0;
            }
            if (t >= 0 && t < 32 && t % 2 == 0) {
                int   step   = t / 2;
                float speed  = 1.0f + step * 0.25f;
                float offset = enemy.m_Mirrored ? (-0.7139983f + step * 0.14279966f)
                                                : (0.7139983f - step * 0.14279966f);
                ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                          EBulletColor::Green, 2, speed, offset, 0.14279966f);
            }
            if (t == 124) TransitionToRandomSub(enemy, ctx, 19, 20, 18);
            break;
        }

        case 23: {  // Phase 2 spellcard — Demarcation (3 Rice pairs w/ curve + RingBall spiral)
            // Each bullet redirects vertically after 40f at speed 1.5 (ECL ins_82 flag 0x40).
            if (t == 0) {
                StartSpellPhase(enemy, ctx, "Demarcation", 4, 3000000, 1500);
                enemy.m_BossMaxLife = enemy.m_Life > 0 ? enemy.m_Life : 1;
            }
            if (t == 120) {
                enemy.m_CanTakeDamage = true;
            }
            {
                // One cycle = 336 frames (3 pairs at 0/60/120, spiral at 180 for 96f, +60 wait)
                int loopT = (t >= 120) ? (t - 120) % 336 : -1;
                if (loopT >= 0) {
                    const glm::vec2 ringAnchor = enemy.m_Pos + glm::vec2{1.0f, 0.0f};
                    auto            spawnPair  = [&](EBulletColor color, bool upFirst) {
                        float a1 = upFirst ? +Util::HALF_PI : -Util::HALF_PI;
                        float a2 = upFirst ? -Util::HALF_PI : +Util::HALF_PI;
                        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ringAnchor, EBulletType::Rice,
                                                                 color, 12, 3.0f, 0.0f, false, 0.0f,
                                                                 {40, a1, 1.5f, true}, true);
                        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ringAnchor, EBulletType::Rice,
                                                                 color, 12, 3.0f, 0.2617994f, false, 0.0f,
                                                                 {40, a2, 1.5f, true}, true);
                    };
                    if (loopT == 0) spawnPair(EBulletColor::Blue, true);
                    if (loopT == 60) spawnPair(EBulletColor::Green, false);
                    if (loopT == 120) spawnPair(EBulletColor::Red, true);
                    if (loopT == 180) {
                        ctx.MoveRandInBounds(enemy);
                        ctx.StartLerpDir(enemy, 2.0f, 120);
                    }
                    // Spiral: 2 outer × (12 neg + 12 pos) × every 2f = 96 frames (loopT 180-275)
                    int spiralT = loopT - 180;
                    if (spiralT >= 0 && spiralT < 96 && spiralT % 2 == 0) {
                        int   phase  = (spiralT % 48) / 24;  // 0 = neg dir, 1 = pos dir
                        int   step   = (spiralT % 24) / 2;   // 0..11
                        float speed  = 1.0f + step * 0.2f;
                        float offset = (phase == 0 ? -0.57119864f + step * 0.14279966f
                                                   : +0.57119864f - step * 0.14279966f);
                        ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                                  EBulletColor::Blue, 1, speed, offset, 0.09817477f,
                                                  false, true);
                    }
                    if (loopT == 276) {
                        ctx.MoveRandInBounds(enemy);
                        ctx.StartLerpDir(enemy, 2.0f, 60);
                    }
                }
            }
            break;
        }

        case 17: {  // Boss death — clear field, despawn after 60f
            if (t == 0) {
                enemy.m_CanTakeDamage  = false;
                enemy.m_InSpellcard    = false;
                enemy.m_ShowSpellName  = false;
                enemy.m_SpellcardBonus = 0;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 0.6f, 60);
            }
            if (t == 90) {
                enemy.m_Alive = false;
            }
            break;
        }

        default:
            break;
    }
}
