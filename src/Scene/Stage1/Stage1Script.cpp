#include "Scene/Stage1/Stage1Script.hpp"

#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

void Stage1Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

void Stage1Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    int offset = Anm::STG1ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 1:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            break;

        case 2:
            ctx.anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;
            break;

        case 3:
            ctx.anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;
            break;

        case 8: {
            ctx.anm.SetScript(enemy.m_Vm, offset + 128, offset);
            enemy.m_HitboxSize             = {48, 56};
            enemy.m_ItemDrop               = -1;
            enemy.m_ItemDropCount          = 0;
            enemy.m_Pos                    = Util::GameFieldToScreen(192.0f, -32.0f);
            enemy.m_IsBoss                 = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_BossMaxLife            = 6000;
            enemy.m_DeathCallbackSub       = 6;
            enemy.m_LifeCallbackThreshold  = -1;
            enemy.m_LifeCallbackSub        = -1;
            enemy.m_TimerCallbackThreshold = 1440;
            enemy.m_TimerCallbackSub       = 7;
            enemy.m_BlocksTimeline         = true;
            break;
        }

        case 10: {
            int off2 = Anm::STG1ENM2.offset;
            ctx.anm.SetScript(enemy.m_Vm, off2 + 128, off2);
            enemy.m_HitboxSize       = {48, 56};
            enemy.m_ItemDrop         = -1;
            enemy.m_ItemDropCount    = 0;
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

void Stage1Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    int   t      = enemy.m_FrameTimer;
    int   offset = Anm::STG1ENM.offset;
    float dir    = enemy.m_Mirrored ? -1.0f : 1.0f;

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
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 70) {
                glm::vec2 shootPos = enemy.m_Pos + glm::vec2{12.0f, -12.0f};
                ctx.bullets.SpawnFanAimed(shootPos, ctx.playerPos, EBulletType::RingBall,
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

        case 7: {
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
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

        case 6: {
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Speed = 0.0f;
                for (int k = 0; k < 5; k++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 120);
            }
            if (t == 120) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    ctx.renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }

        case 8: {
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
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                                                      kSub4Colors[wave], 14, 0.0f, kSub4Offsets[wave], true,
                                                                      0.02f);
            };

            auto randSpeed = [&]() { return (rand() % 1000) / 1000.0f * 3.0f + 0.5f; };
            auto randAngle = [&]() {
                return ((rand() % 1000) / 1000.0f * 2.0f - 1.0f) * 3.14159265f;
            };
            auto sub5Wave = [&](int wave) {
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Pellet, kSub4Colors[wave], 8,
                                        randSpeed(), randAngle(), true);
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Rice, kSub4Colors[wave], 8,
                                        randSpeed(), randAngle(), true);
            };

            if (t == 0) ctx.StartLerpTo(enemy, 320.0f, 128.0f, 60);
            if (t == 60) enemy.m_CanTakeDamage = true;
            if (t == 160) circle3(EBulletColor::Blue);
            if (t == 202) ctx.StartLerpTo(enemy, 192.0f, 64.0f, 60);
            for (int w = 0; w < 5; w++) {
                if (t == 262 + w * 8) sub4Wave(w);
            }
            if (t == 384) ctx.StartLerpTo(enemy, 64.0f, 96.0f, 60);
            if (t == 414) circle3(EBulletColor::Green);
            if (t == 444) circle3(EBulletColor::Yellow);
            if (t == 526) ctx.StartLerpTo(enemy, 192.0f, 80.0f, 60);
            for (int w = 0; w < 5; w++) {
                if (t == 586 + w * 8) sub5Wave(w);
            }
            if (t == 708) ctx.StartLerpTo(enemy, 320.0f, 96.0f, 60);
            if (t == 738) circle3(EBulletColor::Blue);
            if (t == 768) circle3(EBulletColor::Red);
            if (t == 850) ctx.StartLerpTo(enemy, 192.0f, 64.0f, 60);
            for (int w = 0; w < 5; w++) {
                if (t == 910 + w * 8) sub4Wave(w);
            }
            if (t == 1032) ctx.StartLerpTo(enemy, 64.0f, 96.0f, 60);
            if (t == 1062) circle3(EBulletColor::Green);
            if (t == 1092) circle3(EBulletColor::Yellow);
            if (t == 1174) ctx.StartLerpTo(enemy, 192.0f, 80.0f, 60);
            for (int w = 0; w < 5; w++) {
                if (t == 1234 + w * 8) sub5Wave(w);
            }
            if (t == 1356) ctx.StartLerpTo(enemy, 320.0f, 96.0f, 60);
            if (t == 1386) circle3(EBulletColor::Blue);
            if (t == 1416) circle3(EBulletColor::Red);
            break;
        }

        case 10: {
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
            if (t == 100) ctx.TransitionToSub(enemy, 12);
            break;
        }

        case 12: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
            }
            if (t >= 12 && t <= 60 && (t - 12) % 8 == 0) {
                float speed1 = 4.0f;
                ctx.bullets.SpawnFanStack(
                    enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
                    (((t - 12) / 8) % 2 == 0) ? EBulletColor::Red : EBulletColor::DarkRed, 1, 10,
                    speed1, 1.0f, 0.0f, 0.09817477f);
            }
            if (t == 180) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 13 : (r == 1 ? 14 : 15));
            }
            break;
        }

        case 13: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
            }
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
                                             EBulletColor::Blue, 12, 2.0f);
            }
            if (t == 84) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::DarkBlue, 16, 3.0f);
            }
            if (t == 92) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 12, 4.0f);
            }
            if (t == 100) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::DarkBlue, 16, 3.0f);
            }
            if (t == 108) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 12, 2.0f);
            }
            if (t == 228) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 12 : (r == 1 ? 14 : 15));
            }
            break;
        }

        case 14: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 80) {
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Red, 3, 16, 5.0f, 1.0f, 0.0f, 0.06544985f);
            }
            if (t == 110) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 16, 2.0f);
            }
            if (t == 200) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 13 : (r == 1 ? 12 : 15));
            }
            break;
        }

        case 15: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
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
            if (t == 124) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 13 : (r == 1 ? 14 : 12));
            }
            break;
        }

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
                for (int k = 0; k < 5; k++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
            }
            if (t == 200) {
                enemy.m_CanTakeDamage = true;
                ctx.TransitionToSub(enemy, 18);
            }
            break;
        }

        case 17: {
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
            }
            if (t == 60) {
                enemy.m_Alive = false;
            }
            break;
        }

        case 18: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 12) {
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                          EBulletColor::Green, 2, 8, 3.0f, 1.0f, 0.0f, 0.09817477f);
            }
            if (t >= 20 && t <= 60 && (t - 20) % 8 == 0) {
                ctx.lasers.SpawnAimed(enemy.m_Pos, ctx.playerPos, 500.0f, 16.0f, 120, 60, 14, 16,
                                      120);
            }
            if (t == 224) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 19 : (r == 1 ? 20 : 21));
            }
            break;
        }

        case 19: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 60) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                             EBulletColor::Green, 36, 2.0f);
            }
            if (t == 90) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                             EBulletColor::Green, 28, 2.6f);
            }
            if (t == 120) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                             EBulletColor::Green, 36, 2.0f);
            }
            if (t == 240) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 18 : (r == 1 ? 20 : 21));
            }
            break;
        }

        case 20: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
            }
            if (t == 60)
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Yellow, 8, 2, 3.0f, 1.0f, 0.0f, 0.2617994f);
            if (t == 80)
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Yellow, 9, 2, 3.0f, 1.0f, 0.0f, 0.2617994f);
            if (t == 100)
                ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                          EBulletColor::Yellow, 10, 2, 3.0f, 1.0f, 0.0f,
                                          0.2617994f);
            if (t == 220) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 19 : (r == 1 ? 18 : 21));
            }
            break;
        }

        case 21: {
            if (t == 0) {
                ctx.MoveRandInBounds(enemy);
                ctx.StartLerpDir(enemy, 3.0f, 60);
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
            if (t == 124) {
                int r = rand() % 3;
                ctx.TransitionToSub(enemy, r == 0 ? 19 : (r == 1 ? 20 : 18));
            }
            break;
        }

        case 22: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 1500;
                ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
            }
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
                    shootSpiral(0, EBulletColor::DarkBlue, -0.57119864f, +0.14279966f);
                    shootSpiral(32, EBulletColor::DarkCyan, +0.57119864f, -0.14279966f);
                    shootSpiral(64, EBulletColor::Blue, -0.7853982f, +0.19634955f);
                    shootSpiral(96, EBulletColor::Cyan, +0.7853982f, -0.19634955f);
                    shootSpiral(128, EBulletColor::DarkBlue, -0.57119864f, +0.14279966f);
                    shootSpiral(160, EBulletColor::DarkCyan, +0.57119864f, -0.14279966f);
                    shootSpiral(192, EBulletColor::Blue, -0.7853982f, +0.19634955f);
                    shootSpiral(224, EBulletColor::Cyan, +0.7853982f, -0.19634955f);
                    if (loopT == 256) {
                        ctx.MoveRandInBounds(enemy);
                        ctx.StartLerpDir(enemy, 2.0f, 120);
                    }
                }
            }
            break;
        }

        case 23: {
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 1500;
                ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
            }
            if (t == 120) {
                enemy.m_CanTakeDamage = true;
            }
            {
                int loopT = (t >= 120) ? (t - 120) % 360 : -1;
                if (loopT >= 0) {
                    auto spawnPair = [&](EBulletColor color) {
                        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                                     color, 16, 3.0f, 0.0f);
                        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                                     color, 16, 3.0f, 0.19634955f);
                    };
                    if (loopT == 0) spawnPair(EBulletColor::Blue);
                    if (loopT == 60) spawnPair(EBulletColor::Green);
                    if (loopT == 120) spawnPair(EBulletColor::Red);
                    if (loopT == 180) spawnPair(EBulletColor::Green);
                    if (loopT == 240) spawnPair(EBulletColor::Red);
                    if (loopT == 300) {
                        ctx.MoveRandInBounds(enemy);
                        ctx.StartLerpDir(enemy, 2.0f, 120);
                    }
                }
            }
            break;
        }

        default:
            break;
    }
}
