#include "Scene/Stage4/Stage4Script.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace {
constexpr float     PI                     = 3.14159265f;
constexpr glm::vec2 PATCHOULI_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil = EnemyScriptUtil;

constexpr int SUB_LIBRARY_FAIRY_BURST       = 1;
constexpr int SUB_KOAKUMA_MIDBOSS           = 21;
constexpr int SUB_KOAKUMA_DEATH             = 24;
constexpr int SUB_KOAKUMA_ESCAPE            = 25;
constexpr int SUB_PATCHOULI_ENTRY           = 26;
constexpr int SUB_PATCHOULI_FIRST_NONSPELL  = 27;
constexpr int SUB_PATCHOULI_SECOND_NONSPELL = 37;
constexpr int SUB_PATCHOULI_FINAL_NONSPELL  = 39;
constexpr int SUB_PATCHOULI_PHASE_OUT       = 40;
constexpr int SUB_PATCHOULI_PRINCESS_UNDINE = 45;
constexpr int SUB_PATCHOULI_SYLPHY_HORN_ADV = 48;
constexpr int SUB_PATCHOULI_WATER_ELF       = 56;
constexpr int SUB_PATCHOULI_DEATH           = 60;

float RandAngle() {
    return ScriptUtil::RandFloat(-PI, PI);
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = PATCHOULI_SHOOT_OFFSET) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetPatchouliBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, Anm::STG4ENM.offset + 65, Anm::STG4ENM.offset + 69,
                             Anm::STG4ENM.offset + 70, Anm::STG4ENM.offset + 67,
                             Anm::STG4ENM.offset + 68);
}

void SpawnAtEnemyFieldPos(const Enemy& enemy, EnemySubCtx& ctx, int subId, int life, int score,
                          int itemDrop) {
    ctx.SpawnEnemy(subId, enemy.m_Pos.x - Util::FIELD_OFFSET_X,
                   enemy.m_Pos.y - Util::FIELD_OFFSET_Y, life, score, false, itemDrop);
}

void StartPatchouliPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                         int lifeCount, int timerFrames, int nextSub, int lifeThreshold,
                         bool spell) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  nextSub,
                                  lifeThreshold,
                                  nextSub,
                                  spell,
                                  spell,
                                  0,
                                  false,
                                  true,
                              });
}

void StartPatchouliFinalPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                              int lifeThreshold, int lifeSub) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  0,
                                  2400,
                                  SUB_PATCHOULI_DEATH,
                                  SUB_PATCHOULI_DEATH,
                                  lifeThreshold,
                                  lifeSub,
                                  true,
                                  true,
                                  0,
                                  false,
                                  true,
                              });
}

void SpawnAimedRiceFan(Enemy& enemy, EnemySubCtx& ctx, int t, int count, float speed,
                       float spread, int stacks = 2, float innerSpeed = -1.0f) {
    if (t < 0) return;
    ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice, EBulletColor::Blue,
                              count, stacks, speed,
                              innerSpeed >= 0.0f ? innerSpeed : std::max(0.4f, speed - 0.8f),
                              0.0f, spread, true);
}

void SpawnAimedCircleStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                           int count, int stacks, float speed1, float speed2,
                           float aimOffset = 0.0f, bool rotateWithAngle = false,
                           float acceleration = 0.0f, int accelerationFrames = 0,
                           float angularVelocity = 0.0f, int angularVelocityFrames = 0) {
    for (int s = 0; s < stacks; s++) {
        const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
        const float speed = speed1 + (speed2 - speed1) * lerp;
        ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, type, color, count, speed, aimOffset,
                                     false, acceleration, accelerationFrames, {},
                                     rotateWithAngle, angularVelocity, angularVelocityFrames);
    }
}

void RunStage4SmallFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        switch (enemy.m_SubId) {
            case 2:
            case 4:
            case 6:
            case 9:
                enemy.m_Angle = enemy.m_Mirrored ? PI : 0.0f;
                break;
            case 3:
            case 5:
            case 7:
                enemy.m_Angle = enemy.m_Mirrored ? -2.0943952f : -1.0471976f;
                break;
            case 8:
                enemy.m_Angle = Util::HALF_PI;
                break;
            default:
                break;
        }
        enemy.m_Speed = (enemy.m_SubId == 2 || enemy.m_SubId == 3 || enemy.m_SubId == 4)
                            ? 4.2f
                            : 4.5f;
    }

    if (t == 30) {
        if (enemy.m_SubId == 2 || enemy.m_SubId == 4 || enemy.m_SubId == 6) {
            enemy.m_AngularVelocity = enemy.m_Mirrored ? 0.06544985f : -0.06544985f;
        } else if (enemy.m_SubId == 3 || enemy.m_SubId == 5 || enemy.m_SubId == 7) {
            enemy.m_AngularVelocity = enemy.m_Mirrored ? -0.06544985f : 0.06544985f;
        }
    }
    if (t == 70 && (enemy.m_SubId == 2 || enemy.m_SubId == 4 || enemy.m_SubId == 6)) {
        enemy.m_AngularVelocity = 0.0f;
    }
    if (t == 70 && (enemy.m_SubId == 3 || enemy.m_SubId == 5)) {
        enemy.m_AngularVelocity = enemy.m_Mirrored ? 0.1308997f : -0.1308997f;
    }
    if (t == 110 && enemy.m_SubId == 7) {
        enemy.m_AngularVelocity = 0.0f;
    }
    if (t == 150 && (enemy.m_SubId == 3 || enemy.m_SubId == 5)) {
        enemy.m_AngularVelocity = 0.0f;
    }

    switch (enemy.m_SubId) {
        case 2:
            if (t == 50) SpawnAimedRiceFan(enemy, ctx, t, 3, 1.8f, 0.15707964f, 2, 1.0f);
            break;
        case 3:
            if (t == 50 || t == 100) {
                SpawnAimedRiceFan(enemy, ctx, t, 3, 1.8f, 0.15707964f, 2, 1.0f);
            }
            break;
        case 4:
            if (t == 50) SpawnAimedRiceFan(enemy, ctx, t, 1, 1.5f, 0.17453292f, 3, 0.7f);
            break;
        case 5:
            if (t == 50 || t == 128) {
                SpawnAimedRiceFan(enemy, ctx, t, 1, 1.5f, 0.17453292f, 3, 0.7f);
            }
            break;
        case 8:
        case 9:
            if (t == 50) SpawnAimedRiceFan(enemy, ctx, t, 3, 2.0f, 0.17453292f, 2, 1.2f);
            break;
        default:
            break;
    }
}

void RunBookFairy(Enemy& enemy, EnemySubCtx& ctx, int t, int burstSub) {
    if (t == 40) enemy.m_Acceleration = -0.06666667f;
    if (t == 70) {
        SpawnAtEnemyFieldPos(enemy, ctx, burstSub, 1000, 10, static_cast<int>(ItemType::PowerSmall));
        enemy.m_Acceleration = 0.0f;
        enemy.m_Angle        = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed        = 1.8f;
    }
}

void RunBurstFamiliar(Enemy& enemy, EnemySubCtx& ctx, int t, bool laser, bool denseKunai) {
    enemy.m_CanTakeDamage = false;
    const int interval = denseKunai ? 2 : 6;
    const int volleys  = denseKunai ? 36 : 12;
    if (!laser && t >= 30 && t < 30 + interval * volleys && (t - 30) % interval == 0) {
        const glm::vec2 pos = enemy.m_Pos + glm::vec2{ScriptUtil::RandFloat(-40.0f, 40.0f),
                                                      ScriptUtil::RandFloat(-40.0f, 40.0f)};
        if (denseKunai) {
            ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Kunai,
                                      EBulletColor::Blue, 4, 2.0f, 0.0f, 0.09817477f, false,
                                      true);
        } else {
            ctx.bullets.SpawnCircle(pos, EBulletType::Kunai, EBulletColor::Blue, 4,
                                    ScriptUtil::RandFloat(1.0f, 2.0f), RandAngle(), false,
                                    0.0f, 0, true);
        }
    }
    if (t == 120 && laser) {
        ctx.lasers.SpawnAtAngle(enemy.m_Pos, Util::HALF_PI, 500.0f, 32.0f, 90, 120, 16, 70, 14);
    }
    if (t >= (laser ? 440 : 156)) enemy.m_Alive = false;
}

void RunRingCaster(Enemy& enemy, EnemySubCtx& ctx, int t, bool sideEntry) {
    if (t == 40) enemy.m_Acceleration = -0.06666667f;
    if (t == 70) enemy.m_Acceleration = 0.0f;
    if (t >= 70 && t < 70 + 64 * 8 && (t - 70) % 8 == 0) {
        const int   volley = (t - 70) / 8;
        const float sign   = enemy.m_Mirrored ? -1.0f : 1.0f;
        ctx.bullets.SpawnCircleStack(enemy.m_Pos, EBulletType::Rice, EBulletColor::Blue,
                                     3, 2, sideEntry ? 2.1f : 2.6f,
                                     sideEntry ? 1.4f : 1.8f,
                                     RandAngle() + sign * volley * 0.24166097f, false, true);
    }
    if (t == 70 + 64 * 8) {
        enemy.m_Angle        = -Util::HALF_PI;
        enemy.m_Speed        = 2.0f;
    }
}

void RunCrystalFairy(Enemy& enemy, EnemySubCtx& ctx, int t, int maxVolleys, bool green,
                     bool largeBlue) {
    if (t == 30) enemy.m_CanTakeDamage = true;
    if (t >= 30 && t < 30 + maxVolleys * 50 && (t - 30) % 50 == 0) {
        if (green) {
            const float base = RandAngle();
            ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                         EBulletColor::Green, 11, 1.2f, base, false, 0.0f,
                                         0, {}, false);
            ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
                                         EBulletColor::Green, 11, 1.2f, base + 0.28559932f,
                                         false, 0.0f, 0, {}, false);
        } else {
            ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
                                         EBulletColor::Blue, largeBlue ? 24 : 16,
                                         largeBlue ? 2.4f : 1.4f, 0.0f, false, 0.0f, 0, {},
                                         false);
        }
    }
    if (t >= 30 + maxVolleys * 50 + 30) enemy.m_Alive = false;
}

void SpawnPatchouliLaserSet(Enemy&, EnemySubCtx& ctx, float spin, int duration,
                            bool clearFirst) {
    if (clearFirst) ctx.lasers.ClearAll();

    const glm::vec2 laserOrigin = Util::GameFieldToScreen(192.0f, 128.0f);
    const float angles[] = {0.7853982f, 2.3561945f, -2.3561945f, -0.7853982f};
    for (float angle : angles) {
        ctx.lasers.SpawnAtAngle(laserOrigin, angle, 500.0f, 24.0f, 30, duration, 20, 30, 14,
                                spin, 0.0f, 120, 64.0f);
    }
}

void SpawnPatchouliLaserPointBullets(EnemySubCtx& ctx, int cycle, int laserT) {
    if (cycle == 0 || laserT < 0 || laserT >= 180 || (laserT % 50) != 0) return;

    const bool  sub33     = (cycle % 2) == 0;
    const float firstSpin = sub33 ? -0.006829549f : 0.006829549f;
    const float secondSpin = -firstSpin;
    const glm::vec2 origin = Util::GameFieldToScreen(192.0f, 128.0f);
    const float startOffset = 64.0f;
    const float angles[] = {0.7853982f, 2.3561945f, -2.3561945f, -0.7853982f};

    const auto spawnAtAngle = [&](float angle) {
        const glm::vec2 pos =
            origin + glm::vec2{std::cos(angle), std::sin(angle)} * startOffset;
        if (sub33) {
            SpawnAimedCircleStack(ctx, pos, EBulletType::Ball, EBulletColor::Red, 1, 2, 2.0f,
                                  1.2f);
        } else {
            ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, EBulletType::Ball,
                                         EBulletColor::Red, 10, 2.0f, 0.0f, false);
        }
    };

    for (float angle : angles) {
        spawnAtAngle(angle + firstSpin * static_cast<float>(std::min(laserT, 120)));
    }
    if (laserT >= 60) {
        for (float angle : angles) {
            spawnAtAngle(angle + secondSpin * static_cast<float>(std::min(laserT - 60, 120)));
        }
    }
}

void HoldAtFieldCenter(Enemy& enemy) {
    enemy.m_Pos             = Util::GameFieldToScreen(192.0f, 128.0f);
    enemy.m_IsLerping       = false;
    enemy.m_Speed           = 0.0f;
    enemy.m_Acceleration    = 0.0f;
    enemy.m_AngularVelocity = 0.0f;
}

void RunKoakumaMidboss(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ctx.KillAllNonBossEnemies();
        ctx.BulletCancelIntoPointItems();
        enemy.m_Pos = Util::GameFieldToScreen(192.0f, 128.0f);
    }
    if (t == 30) enemy.m_CanTakeDamage = true;

    if (t >= 30) {
        const int loopT = (t - 30) % 441;
        if (loopT == 0 || loopT == 250) enemy.m_LockedShotAngle = RandAngle();

        const auto spawnBubbleRing = [&](int volley, float dir) {
            ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Bubble, EBulletColor::DarkRed,
                                    8, 1.2f, enemy.m_LockedShotAngle + dir * volley, false,
                                    0.024f, 99999, false);
        };

        if (loopT < 160 && loopT % 20 == 0) {
            spawnBubbleRing(loopT / 20, 0.19634955f);
        }
        if (loopT == 160) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
        if (loopT == 160 || loopT == 190 || loopT == 410 || loopT == 440) {
            SpawnAtEnemyFieldPos(enemy, ctx, SUB_LIBRARY_FAIRY_BURST, 1000, 10,
                                 static_cast<int>(ItemType::PowerSmall));
        }
        if (loopT >= 250 && loopT < 410 && (loopT - 250) % 20 == 0) {
            spawnBubbleRing((loopT - 250) / 20, -0.19634955f);
        }
    }
}

void RunPatchouliFirstNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliPhase(enemy, ctx, "Patchouli Knowledge", 17000, 2, 2400,
                            SUB_PATCHOULI_PRINCESS_UNDINE, 1600, false);
        ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
    }
    if (t == 60) enemy.m_CanTakeDamage = true;
    if (t >= 60 && t < 100) HoldAtFieldCenter(enemy);
    if (t < 100) return;

    const int loopT = (t - 100) % 420;
    const int cycle = (t - 100) / 420;
    if (loopT < 180 || loopT >= 360) HoldAtFieldCenter(enemy);

    if (loopT == 0) {
        const bool sub33 = (cycle % 2) == 0;
        SpawnPatchouliLaserSet(enemy, ctx, sub33 ? -0.006829549f : 0.006829549f, 70, true);
    }
    if (loopT == 60) {
        const bool sub33 = (cycle % 2) == 0;
        SpawnPatchouliLaserSet(enemy, ctx, sub33 ? 0.006829549f : -0.006829549f, 90, false);
    }
    SpawnPatchouliLaserPointBullets(ctx, cycle, loopT);
    if (loopT == 180) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    if (loopT >= 180 && loopT < 360 && (loopT - 180) % 40 == 0) {
        SpawnAimedCircleStack(ctx, ShootPos(enemy), EBulletType::Ball, EBulletColor::Red,
                              std::min(26, 16 + cycle), 4, 3.5f, 1.2f);
    }
    if (loopT == 270) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 90);
}

void RunPatchouliSecondNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliPhase(enemy, ctx, "Patchouli Knowledge", 16000, 1, 2400,
                            SUB_PATCHOULI_SYLPHY_HORN_ADV, 1600, false);
        ScriptUtil::DropPowerItems(enemy, ctx, 8);
        ctx.StartLerpTo(enemy, 192.0f, 128.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t >= 120 && t < 180) HoldAtFieldCenter(enemy);
    if (t < 180) return;

    const int loopT = (t - 180) % 420;
    const int cycle = (t - 180) / 420;
    if (loopT < 180 || loopT >= 360) HoldAtFieldCenter(enemy);

    if (loopT == 0) {
        const bool sub33 = (cycle % 2) == 0;
        SpawnPatchouliLaserSet(enemy, ctx, sub33 ? -0.006829549f : 0.006829549f, 70, true);
    }
    if (loopT == 60) {
        const bool sub33 = (cycle % 2) == 0;
        SpawnPatchouliLaserSet(enemy, ctx, sub33 ? 0.006829549f : -0.006829549f, 90, false);
    }
    SpawnPatchouliLaserPointBullets(ctx, cycle, loopT);
    if (loopT == 180) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    if (loopT >= 180 && loopT < 360 && (loopT - 180) % 15 == 0) {
        SpawnAimedCircleStack(ctx, ShootPos(enemy), EBulletType::Ball, EBulletColor::Blue,
                              std::min(22, 10 + cycle), 3, 4.0f, 1.5f);
    }
    if (loopT == 270) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 90);
    if (loopT == 360) {
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                     EBulletColor::Blue, 12, 1.5f, 0.0f, false);
    }
}

void RunPrincessUndine(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliPhase(enemy, ctx, "Water Sign \"Princess Undine\"", -1, 2, 2100,
                            SUB_PATCHOULI_SECOND_NONSPELL, -1, true);
        ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 250;
    if (loopT < 120 && loopT % 22 == 0) {
        ctx.bullets.SpawnFanStack(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos,
                                  EBulletType::Ball, EBulletColor::Blue, 16, 2, 3.5f, 1.2f,
                                  PI, 0.34906584f, false);
    }
    if (loopT < 120 && loopT % 10 == 0) {
        const float offset = PI / 8.0f - static_cast<float>(loopT / 10) * 0.02617994f;
        const auto  pos    = ShootPos(enemy);
        const float aim    = std::atan2(ctx.playerPos.y - pos.y, ctx.playerPos.x - pos.x);
        ctx.lasers.SpawnAtAngle(pos, aim, 96.0f, 6.0f, 0, 9999, 30, 0, 30, 0.0f, 4.0f);
        ctx.lasers.SpawnAtAngle(pos, aim + offset, 96.0f, 6.0f, 0, 9999, 30, 0, 30, 0.0f,
                                4.0f);
        ctx.lasers.SpawnAtAngle(pos, aim - offset, 96.0f, 6.0f, 0, 9999, 30, 0, 30, 0.0f,
                                4.0f);
    }
    if (loopT >= 120 && loopT < 200 && loopT % 10 == 0) {
        const float drift = static_cast<float>(loopT - 120) * 0.02f;
        ctx.bullets.SpawnFanAimed(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos,
                                  EBulletType::BigBall, EBulletColor::DarkPurple, 10, 2.5f,
                                  drift, 0.22439948f, false, false);
        ctx.bullets.SpawnFanAimed(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos,
                                  EBulletType::Ball, EBulletColor::Blue, 6, 0.8f, -drift,
                                  0.34906584f, false, false);
    }
    if (loopT == 130) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    if (loopT == 210) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

void SpawnEdgeShardRain(EnemySubCtx& ctx, EBulletColor color, bool fromLeft, int count = 1,
                        float speed = 1.2f) {
    for (int i = 0; i < count; i++) {
        const float x = fromLeft ? -12.0f : 396.0f;
        const float y = ScriptUtil::RandFloat(32.0f, 224.0f);
        const float angle = fromLeft ? ScriptUtil::RandFloat(0.45f, 1.15f)
                                     : ScriptUtil::RandFloat(1.99f, 2.70f);
        ctx.bullets.SpawnCircle(Util::GameFieldToScreen(x, y), EBulletType::Shard, color, 1,
                                speed, angle, false, 0.0f, 0, true);
    }
}

void RunSylphyHornAdvanced(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliPhase(enemy, ctx, "Wood Sign \"Sylphy Horn Advanced\"", -1, 1, 2100,
                            SUB_PATCHOULI_FINAL_NONSPELL, -1, true);
        ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 126;
    if (loopT % 24 == 0) {
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Shard,
                                EBulletColor::Yellow, 15, 2.0f, RandAngle(), false, 0.0f, 0,
                                true);
    }
    if (loopT % 4 == 0) {
        const float x = ScriptUtil::RandFloat(0.0f, 384.0f);
        ctx.bullets.SpawnCircle(Util::GameFieldToScreen(x, 32.0f), EBulletType::Shard,
                                EBulletColor::Green, 1, 1.2f, ScriptUtil::RandFloat(1.05f, 1.55f),
                                false, 0.0f, 0, true);
        SpawnEdgeShardRain(ctx, EBulletColor::Lime, true);
    }
}

void RunFinalNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliFinalPhase(enemy, ctx, "Water & Wood Sign \"Water Elf\"", 3400, 1700,
                                 SUB_PATCHOULI_PHASE_OUT);
        ScriptUtil::DropPowerItems(enemy, ctx, 8);
        ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
        enemy.m_SecondaryShotAngle = 0.0f;
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int attackT = t - 120;
    const int loopT   = attackT % 70;
    const int cycle   = attackT / 70;
    const auto pos    = enemy.m_Pos;

    if (loopT == 0) {
        if (cycle % 8 == 0) enemy.m_SecondaryShotAngle = 0.0f;
        SpawnAimedCircleStack(ctx, pos, EBulletType::Ball, EBulletColor::Cyan, 24, 2, 2.8f,
                              1.2f, PI);
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::BigBall,
                                  EBulletColor::DarkPurple, 10, 1.8f,
                                  enemy.m_SecondaryShotAngle, 0.34906584f, false, false);
        enemy.m_SecondaryShotAngle -=
            ScriptUtil::RandFloat(-0.11219974f, 0.11219974f);
    }
    if (loopT == 20) {
        SpawnAimedCircleStack(ctx, pos, EBulletType::Ball, EBulletColor::Green, 20, 2, 2.8f,
                              1.2f, PI);
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
}

void RunMercuryPoison(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliFinalPhase(enemy, ctx, "Metal & Water Sign \"Mercury Poison\"", -1, -1,
                                 -1);
        ScriptUtil::DropPowerItems(enemy, ctx, 8);
        ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int attackT = t - 120;
    const int loopT   = attackT % 370;
    const int cycle   = attackT / 370;
    const int count   = std::min(28, 15 + cycle);
    const auto pos    = enemy.m_Pos;

    if (loopT < 320 && loopT % 40 == 0) {
        enemy.m_LockedShotAngle = RandAngle();
        SpawnAimedCircleStack(ctx, pos, EBulletType::Ball, EBulletColor::Orange, count, 2,
                              1.5f, 0.8f, enemy.m_LockedShotAngle, false, 0.003f, 90,
                              0.012271847f, 90);
    }
    if (loopT < 320 && loopT % 40 == 20) {
        SpawnAimedCircleStack(ctx, pos, EBulletType::Ball, EBulletColor::Cyan, count, 2, 1.5f,
                              0.8f, enemy.m_LockedShotAngle, false, 0.003f, 90,
                              -0.012271847f, 90);
    }
    if (loopT == 320) {
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
}

void RunWaterElf(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartPatchouliPhase(enemy, ctx, "Water & Wood Sign \"Water Elf\"", 3400, 0, 2400,
                            SUB_PATCHOULI_DEATH, -1, true);
        ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 132;
    if (loopT % 40 == 0) {
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Fireball,
                                EBulletColor::Gray, 22, 2.0f, RandAngle(), false, 0.0f, 0);
    }
    if (loopT % 10 == 0) {
        const float x = ScriptUtil::RandFloat(0.0f, 384.0f);
        ctx.bullets.SpawnCircle(Util::GameFieldToScreen(x, 32.0f), EBulletType::Shard,
                                EBulletColor::Red, 1, 1.2f,
                                ScriptUtil::RandFloat(2.08f, 2.64f), false, 0.0f, 0, true);
        SpawnEdgeShardRain(ctx, EBulletColor::Red, false);
    }
    if (loopT == 90) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

}  // namespace

void Stage4Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG4ENM.folder, Anm::STG4ENM.txt, Anm::STG4ENM.offset);
}

void Stage4Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG4ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 18:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 10:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? PI : 0.0f;
            enemy.m_Speed      = 2.0f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case SUB_LIBRARY_FAIRY_BURST:
        case 19:
        case 20:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_HitboxSize    = {24.0f, 24.0f};
            enemy.m_CanTakeDamage = false;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            ctx.anm.SetScript(enemy.m_Vm, offset + 8, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Speed      = 4.5f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 11:
        case 13:
            ctx.anm.SetScript(enemy.m_Vm, offset + 10, offset);
            enemy.m_HitboxSize    = {32.0f, 32.0f};
            enemy.m_Angle         = enemy.m_SubId == 13 ? 0.0f : Util::HALF_PI;
            enemy.m_Speed         = 2.0f;
            enemy.m_DeathCallbackSub = 12;
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case 12:
        case 15:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {1.0f, 1.0f};
            break;

        case 14:
        case 16:
        case 17:
            ctx.anm.SetScript(enemy.m_Vm, offset + (enemy.m_SubId == 16 ? 14 : 13), offset);
            enemy.m_HitboxSize    = {28.0f, 28.0f};
            enemy.m_CanTakeDamage = false;
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case SUB_KOAKUMA_MIDBOSS:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(-32.0f, 170.0f);
            enemy.m_HitboxSize             = {40.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_BlocksTimeline         = false;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossTitle              = "Koakuma";
            enemy.m_BossLifeCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_TimerCallbackThreshold = 2400;
            enemy.m_TimerCallbackSub       = SUB_KOAKUMA_ESCAPE;
            enemy.m_DeathCallbackSub       = SUB_KOAKUMA_DEATH;
            enemy.m_BoundsMin              = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax              = Util::GameFieldToScreen(352.0f, 176.0f);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case SUB_PATCHOULI_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 65, offset);
            enemy.m_Pos            = Util::GameFieldToScreen(64.0f, -32.0f);
            enemy.m_HitboxSize     = {48.0f, 56.0f};
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = true;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Patchouli Knowledge";
            enemy.m_BossLifeCount  = 2;
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 144.0f);
            SetPatchouliBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void Stage4Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;

    switch (enemy.m_SubId) {
        case 0:
        case 10:
            RunBookFairy(enemy, ctx, t, SUB_LIBRARY_FAIRY_BURST);
            break;
        case 18:
            RunBookFairy(enemy, ctx, t, 20);
            break;

        case SUB_LIBRARY_FAIRY_BURST:
            RunBurstFamiliar(enemy, ctx, t, false, false);
            break;
        case 19:
            RunBurstFamiliar(enemy, ctx, t, false, true);
            break;
        case 20:
            RunBurstFamiliar(enemy, ctx, t, true, false);
            break;

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            RunStage4SmallFairy(enemy, ctx, t);
            break;

        case 11:
            RunRingCaster(enemy, ctx, t, false);
            break;
        case 13:
            RunRingCaster(enemy, ctx, t, true);
            break;
        case 12:
            if (t == 0) {
                ctx.BulletCancelIntoPointItems();
                ScriptUtil::DropPowerItems(enemy, ctx, 3);
                enemy.m_Alive = false;
            }
            break;

        case 14:
            RunCrystalFairy(enemy, ctx, t, 60, true, false);
            break;
        case 16:
            RunCrystalFairy(enemy, ctx, t, 10, false, false);
            break;
        case 17:
            RunCrystalFairy(enemy, ctx, t, 6, true, false);
            break;
        case 15:
            if (t >= 30) enemy.m_Alive = false;
            break;

        case SUB_KOAKUMA_MIDBOSS:
            RunKoakumaMidboss(enemy, ctx, t);
            break;
        case SUB_KOAKUMA_DEATH:
            if (t == 0) {
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                ScriptUtil::DropPowerItems(enemy, ctx, 8);
                ctx.TransitionToSub(enemy, SUB_KOAKUMA_ESCAPE);
            }
            break;
        case SUB_KOAKUMA_ESCAPE:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                ctx.BulletCancelIntoPointItems();
                ctx.lasers.ClearAll();
                enemy.m_Alive = false;
            }
            break;

        case SUB_PATCHOULI_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_PATCHOULI_FIRST_NONSPELL);
            break;
        case SUB_PATCHOULI_FIRST_NONSPELL:
            RunPatchouliFirstNonSpell(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_SECOND_NONSPELL:
            RunPatchouliSecondNonSpell(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_PRINCESS_UNDINE:
            RunPrincessUndine(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_SYLPHY_HORN_ADV:
            RunSylphyHornAdvanced(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_FINAL_NONSPELL:
            RunFinalNonSpell(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_PHASE_OUT:
            RunMercuryPoison(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_WATER_ELF:
            RunWaterElf(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_DEATH:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Alive = false;
            }
            break;

        default:
            break;
    }
}
