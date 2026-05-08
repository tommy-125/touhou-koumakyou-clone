#include "Scene/Stage3/Stage3Script.hpp"

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
constexpr float     PI                   = 3.14159265f;
constexpr glm::vec2 MEILING_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil = EnemyScriptUtil;

constexpr int SUB_MEILING_MIDBOSS_MAIN       = 9;
constexpr int SUB_MEILING_MIDBOSS_SPELL_A    = 13;
constexpr int SUB_MEILING_MIDBOSS_SPELL_B    = 14;
constexpr int SUB_MEILING_MIDBOSS_DEATH      = 15;
constexpr int SUB_MEILING_MIDBOSS_ESCAPE     = 16;
constexpr int SUB_MEILING_SUPPORT_FAIRY      = 25;
constexpr int SUB_MEILING_ENTRY              = 17;
constexpr int SUB_MEILING_FIRST_NONSPELL     = 18;
constexpr int SUB_MEILING_RAINBOW_WIND_CHIME = 29;
constexpr int SUB_MEILING_SECOND_NONSPELL    = 20;
constexpr int SUB_MEILING_FINAL_NONSPELL     = 26;
constexpr int SUB_MEILING_COLORFUL_RAIN_A    = 31;
constexpr int SUB_MEILING_COLORFUL_RAIN_B    = 32;
constexpr int SUB_MEILING_EXTREME_TYPHOON    = 33;
constexpr int SUB_MEILING_DEATH              = 34;
float AngleToPlayer(const Enemy& enemy, const EnemySubCtx& ctx) {
    const glm::vec2 d = ctx.playerPos - enemy.m_Pos;
    return std::atan2(d.y, d.x);
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = MEILING_SHOOT_OFFSET) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetMeilingBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, Anm::STG3ENM.offset + 64, Anm::STG3ENM.offset + 65,
                             Anm::STG3ENM.offset + 65, Anm::STG3ENM.offset + 65,
                             Anm::STG3ENM.offset + 65);
}

void StartNonSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, int life, int lifeCount,
                        int timerFrames, int nextSub, int deathSub) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  "Hong Meiling",
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  deathSub,
                                  std::max(1200, life / 8),
                                  nextSub,
                                  false,
                                  false,
                                  0,
                                  false,
                                  true,
                              });
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int lifeCount,
                     int timerFrames, int nextSub) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  -1,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  nextSub,
                                  -1,
                                  -1,
                                  true,
                                  true,
                                  0,
                                  false,
                                  true,
                              });
    ctx.StartLerpTo(enemy, 192.0f, 64.0f, 120);
}

void SpawnRandomCircle(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                       int count, float speed, float variance = 0.0f,
                       bool rotateWithAngle = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1,
                                std::max(0.1f, speed + ScriptUtil::RandFloat(-variance, variance)),
                                ScriptUtil::RandFloat(-PI, PI), false, 0.0f, 0, rotateWithAngle);
    }
}

void SpawnRandomSpeedRange(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type,
                           EBulletColor color, int count, float minSpeed, float maxSpeed,
                           bool rotateWithAngle = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, ScriptUtil::RandFloat(minSpeed, maxSpeed),
                                ScriptUtil::RandFloat(-PI, PI), false, 0.0f, 0, rotateWithAngle);
    }
}

void SpawnRandomVectorAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                            int count, float maxSpeed, float minSpeed, float accelSpeed,
                            float accelAngle, bool rotateWithAngle = true) {
    const glm::vec2 accel = {std::cos(accelAngle) * accelSpeed, std::sin(accelAngle) * accelSpeed};
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, ScriptUtil::RandFloat(minSpeed, maxSpeed),
                                ScriptUtil::RandFloat(-PI, PI), true, 0.0f, 0, rotateWithAngle, accel,
                                9999, 12, 0.5f);
    }
}

void SpawnRandomDownAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                          int count, float maxSpeed, float minSpeed,
                          bool rotateWithAngle = true) {
    SpawnRandomVectorAccel(pos, ctx, type, color, count, maxSpeed, minSpeed, 0.027f,
                           Util::HALF_PI, rotateWithAngle);
}

void RunOpeningFairy(Enemy& enemy, int t) {
    if (t == 30) {
        enemy.m_AngularVelocity = enemy.m_Mirrored ? 0.06544985f : -0.06544985f;
    }
    if (t == 115) enemy.m_AngularVelocity = 0.0f;
}

void RunSideFairy(Enemy& enemy, int t) {
    if (t == 30) {
        enemy.m_AngularVelocity = enemy.m_Mirrored ? -0.034906585f : 0.034906585f;
    }
    if (t == 90) enemy.m_AngularVelocity = 0.0f;
}

void RunMediumKunai(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        enemy.m_LockedShotAngle = AngleToPlayer(enemy, ctx);
    }
    if (t >= 70 && t < 130 && (t - 70) % 2 == 0) {
        const int   step  = (t - 70) / 2;
        const float speed = 1.6f + static_cast<float>(step) * 0.21f;
        ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Kunai, EBulletColor::Blue, 1, speed,
                                enemy.m_LockedShotAngle, false, 0.0f, 0, true);
    }
    if (t == 130) {
        enemy.m_Angle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
}

void RunMediumRing(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        enemy.m_ExitMoveAngle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                  EBulletColor::Red, 16, ScriptUtil::RandFloat(1.0f, 2.0f), 0.0f,
                                  0.2617994f);
    }
    if (t == 100) {
        enemy.m_Angle = enemy.m_ExitMoveAngle + PI;
        enemy.m_Speed = 1.5f;
    }
}

void RunBlueScatter(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_AngularVelocity = enemy.m_Mirrored ? 0.024543693f : -0.024543693f;
    if (t == 120) enemy.m_AngularVelocity = enemy.m_Mirrored ? -0.019634955f : 0.019634955f;
    if (t == 190) {
        ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                  EBulletColor::DarkRed, 3, 1.5f, 0.0f, 0.05235988f, false,
                                  true);
    }
    if (t == 220) enemy.m_AngularVelocity = 0.0f;
    if (t == 280) {
        enemy.m_Angle = enemy.m_Mirrored ? 2.3561945f : 0.7853982f;
        enemy.m_Speed = 2.5f;
    }
}

void RunFixedDownKunaiFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        enemy.m_ExitMoveAngle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
    }
    if (t >= 70 && t < 102 && (t - 70) % 2 == 0) {
        const int   step  = (t - 70) / 2;
        const float speed = 1.6f + static_cast<float>(step) * 0.3f;
        ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Kunai, EBulletColor::Blue, 1, speed,
                                Util::HALF_PI, false, 0.0f, 0, true);
    }
    if (t == 102) {
        enemy.m_Angle = enemy.m_ExitMoveAngle + PI;
        enemy.m_Speed = 1.5f;
    }
}

void RunWhiteRandomBalls(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.083333336f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        SpawnRandomCircle(enemy.m_Pos, ctx, EBulletType::Ball, EBulletColor::White, 14, 2.0f,
                          0.3f);
        enemy.m_Angle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
}

void RunMeilingMidbossPattern(Enemy& enemy, EnemySubCtx& ctx, int frame) {
    static constexpr int BLUE_CYCLE_FRAMES = 230;
    static constexpr int RED_CYCLE_FRAMES  = 120;
    const int            pairT = frame % (BLUE_CYCLE_FRAMES + RED_CYCLE_FRAMES);

    if (pairT < BLUE_CYCLE_FRAMES) {
        if (pairT >= 20 && pairT < 84 && (pairT - 20) % 8 == 0) {
            const glm::vec2 pos = ShootPos(enemy, {ScriptUtil::RandFloat(-16.0f, 16.0f),
                                                   ScriptUtil::RandFloat(-16.0f, 16.0f)});
            SpawnRandomSpeedRange(pos, ctx, EBulletType::Ball, EBulletColor::Blue, 16, 1.7f,
                                  3.0f);
        }
        if (pairT == 124) ScriptUtil::StartRandomMove(enemy, ctx, 5.0f, 50);
    } else {
        const int redT = pairT - BLUE_CYCLE_FRAMES;
        if (redT == 0) {
            ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::RingBall,
                                    EBulletColor::Red, 76, 1.1f);
        }
        if (redT == 10) ScriptUtil::StartRandomMove(enemy, ctx, 5.0f, 50);
    }
}

void RunMeilingMidbossSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_CanTakeDamage = false;
        enemy.m_ShowSpellName = true;
        enemy.m_InSpellcard   = true;
        enemy.m_BossTitle     = "Flower Sign \"Gorgeous Sweet Flower\"";
        enemy.m_BossTimer     = 0;
        enemy.m_TimerCallbackThreshold = 1200;
        enemy.m_TimerCallbackSub       = SUB_MEILING_MIDBOSS_ESCAPE;
        enemy.m_DeathCallbackSub       = SUB_MEILING_MIDBOSS_DEATH;
        enemy.m_LockedShotAngle        = ScriptUtil::RandFloat(-PI, PI);
        enemy.m_SecondaryShotAngle     = ScriptUtil::RandFloat(-PI, PI);
        ctx.BulletCancelIntoPointItems();
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int rel = t - 120;
    if (rel % 6 == 0) {
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Shard,
                                EBulletColor::Yellow, 4, 2.0f, enemy.m_LockedShotAngle, false,
                                0.0f, 0, true);
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Shard,
                                EBulletColor::Yellow, 4, 2.0f, enemy.m_SecondaryShotAngle, false,
                                0.0f, 0, true);
        enemy.m_LockedShotAngle += 0.1134464f;
        enemy.m_SecondaryShotAngle -= 0.1308997f;
    }

    if (rel % 80 == 0) {
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos,
                                     EBulletType::Shard, EBulletColor::Red, 42, 1.2f, 0.0f,
                                     false, 0.0f, 0, {}, true);
    }
}

void RunMeilingFirstNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 50) % 272;
    const int cycle = (t - 50) / 272;

    if (loopT == 0) {
        enemy.m_LockedShotAngle = ScriptUtil::RandFloat(-PI, PI);
    }

    if (loopT < 30 * 4 && loopT % 4 == 0) {
        const int   burst       = loopT / 4;
        const int   count       = std::clamp(5 + cycle, 5, 10);
        const float turnSign    = (cycle % 2 == 0) ? 1.0f : -1.0f;
        const float speed       = 2.0f + burst * 0.05f;
        const float aimOffset   = enemy.m_LockedShotAngle + turnSign * 0.09817477f * burst;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Shard,
                                     EBulletColor::Red, count, speed, aimOffset, false,
                                     0.0f, 0, {}, true);
    }
    if (loopT == 120) {
        ScriptUtil::StartRandomMove(enemy, ctx, 5.0f, 40);
    }
    if (loopT >= 120 && loopT < 152 && loopT % 2 == 0) {
        const int   burst       = (loopT - 120) / 2;
        const int   count       = std::clamp(5 + cycle, 5, 10);
        const float turnSign    = (cycle % 2 == 0) ? 1.0f : -1.0f;
        const float driftStep   = 0.03926991f + 0.06544985f * burst;
        const float speed       = 3.5f + burst * 0.06f;
        const float aimOffset   = enemy.m_LockedShotAngle + turnSign * driftStep * burst;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Shard,
                                     EBulletColor::Red, count, speed, aimOffset, false,
                                     0.0f, 0, {}, true);
    }
}

void RunRainbowWindChimeSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) {
        enemy.m_CanTakeDamage = true;
        ctx.StartLerpTo(enemy, 192.0f, 160.0f, 2000);
    }
    if (t < 120 || (t - 120) % 7 != 0) return;

    const int       volley = (t - 120) / 7;
    const glm::vec2 pos    = ShootPos(enemy, {0.0f, 0.0f});
    float           angle  = enemy.m_LockedShotAngle;

    struct Ring {
        EBulletColor color;
        int          count;
    };
    const Ring rings[] = {{EBulletColor::Red, 2},    {EBulletColor::Orange, 3},
                          {EBulletColor::Yellow, 4}, {EBulletColor::Lime, 2},
                          {EBulletColor::Cyan, 4},   {EBulletColor::Blue, 3},
                          {EBulletColor::Purple, 2}};

    for (const Ring& ring : rings) {
        ctx.bullets.SpawnCircle(pos, EBulletType::Shard, ring.color, ring.count, 2.6f, angle,
                                false, 0.0f, 0, true);
        angle += 0.1134464f;
    }

    if (volley % 96 < 48) {
        enemy.m_LockedShotAngle += 0.1308997f;
    } else {
        enemy.m_LockedShotAngle -= 0.1308997f;
    }
    if (enemy.m_LockedShotAngle > PI) {
        enemy.m_LockedShotAngle -= 2.0f * PI;
    } else if (enemy.m_LockedShotAngle < -PI) {
        enemy.m_LockedShotAngle += 2.0f * PI;
    }
}

void SpawnMeilingSupportFairies(EnemySubCtx& ctx, int wave) {
    struct Spawn {
        float x;
        int   life;
    };
    static constexpr Spawn kWaves[3][2] = {
        {{32.0f, 2000}, {352.0f, 1000}},
        {{96.0f, 500},  {288.0f, 2000}},
        {{160.0f, 1000}, {224.0f, 500}},
    };

    for (const Spawn& spawn : kWaves[wave]) {
        ctx.SpawnEnemy(25, spawn.x, -32.0f, spawn.life, 2000, false,
                       static_cast<int>(ItemType::Point));
    }
}

void RunMeilingSupportFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.05f;

    for (int cycle = 0; cycle < 4; cycle++) {
        const int start = 70 + cycle * 220;
        if (t == start) {
            enemy.m_LockedShotAngle = AngleToPlayer(enemy, ctx);
            enemy.m_Acceleration    = 0.0f;
            enemy.m_Speed           = 0.0f;
        }
        if (t >= start && t < start + 60 && (t - start) % 2 == 0) {
            const int   step  = (t - start) / 2;
            const float speed = 1.6f + step * 0.18f;
            ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Kunai, EBulletColor::Blue, 1,
                                    speed, enemy.m_LockedShotAngle, false, 0.0f, 0, true);
        }
    }

    if (t == 950) {
        enemy.m_Angle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
    if (t >= 10000) enemy.m_Alive = false;
}

void RunMeilingSecondNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 160) % 400;

    if (loopT == 0) SpawnMeilingSupportFairies(ctx, 0);
    if (loopT == 30) SpawnMeilingSupportFairies(ctx, 1);
    if (loopT == 60) SpawnMeilingSupportFairies(ctx, 2);

    if (loopT == 0) {
        enemy.m_LockedShotAngle = ScriptUtil::RandFloat(-PI, PI);
    }
    if (loopT == 0 || loopT == 70 || loopT == 140) {
        ctx.bullets.SpawnCircleStack(ShootPos(enemy), EBulletType::Ball,
                                     loopT == 70 ? EBulletColor::Blue : EBulletColor::DarkBlue,
                                     16, 2, 3.0f, 2.0f, ScriptUtil::RandFloat(-PI, PI), false, false);
    }

    if (loopT == 200 || loopT == 270 || loopT == 340) {
        const int count = loopT == 270 ? 16 : 14;
        SpawnRandomDownAccel(ShootPos(enemy), ctx, EBulletType::Ball, EBulletColor::Blue, count,
                             2.0f, 0.3f, false);
    }

    if (loopT == 120 || loopT == 320) {
        ScriptUtil::StartRandomMove(enemy, ctx, 2.0f, 80);
    }
    if (loopT == 240 || loopT == 310 || loopT == 380) {
        const int count = loopT == 310 ? 16 : 24;
        SpawnRandomSpeedRange(ShootPos(enemy), ctx, EBulletType::Ball, EBulletColor::DarkBlue,
                              count, 2.7f, 5.3f, false);
    }
}

void RunMeilingFinalNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 160) % 280;

    if (loopT == 0 || loopT == 70) {
        const bool blueBurst = loopT == 0;
        BulletCurve curve;
        curve.at       = 60;
        curve.times    = 1;
        curve.angle    = blueBurst ? -2.8972466f : 2.8972466f;
        curve.speed    = 3.0f;
        curve.relative = true;

        const EBulletColor color = blueBurst ? EBulletColor::Blue : EBulletColor::Red;
        for (int i = 0; i < 6; i++) {
            ctx.bullets.SpawnCircle(ShootPos(enemy), EBulletType::Kunai, color, 24,
                                    2.0f + i * 0.6f, 0.0f, false, 0.0f, 0, true,
                                    {0.0f, 0.0f}, 0, 0, 1.0f, curve);
        }
    }
    if (loopT >= 22 && loopT < 102 && loopT % 2 == 0) {
        SpawnRandomDownAccel(ShootPos(enemy), ctx, EBulletType::Shard, EBulletColor::Blue, 1,
                             1.0f, 0.3f, true);
    }
    if (loopT == 160) {
        ScriptUtil::StartRandomMove(enemy, ctx, 4.0f, 80);
    }
}

void RunColorfulRainSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 260;
    if (loopT < 80 && loopT % 4 == 0) {
        SpawnRandomDownAccel(ShootPos(enemy, {0.0f, 0.0f}), ctx, EBulletType::Shard,
                             EBulletColor::Blue, 1, 1.0f, 0.3f, true);
        SpawnRandomDownAccel(ShootPos(enemy, {0.0f, 0.0f}), ctx, EBulletType::Shard,
                             EBulletColor::Green, 1, 1.0f, 0.3f, true);
        SpawnRandomDownAccel(ShootPos(enemy, {0.0f, 0.0f}), ctx, EBulletType::Shard,
                             EBulletColor::Lime, 1, 1.0f, 0.3f, true);
        SpawnRandomDownAccel(ShootPos(enemy, {0.0f, 0.0f}), ctx, EBulletType::Shard,
                             EBulletColor::Cyan, 1, 1.0f, 0.3f, true);
    }
    if (loopT == 100) {
        ScriptUtil::StartRandomMove(enemy, ctx, 3.0f, 80);
    }
    if (loopT >= 120 && loopT < 180 && loopT % 3 == 0) {
        const glm::vec2 pos = ShootPos(enemy, {0.0f, 0.0f});
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Red, 1, 1.0f,
                               0.3f, 0.024f, PI, true);
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Purple, 1, 1.0f,
                               0.3f, 0.024f, 0.0f, true);
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Yellow, 1, 1.0f,
                               0.3f, 0.024f, 2.3561945f, true);
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Orange, 1, 1.0f,
                               0.3f, 0.024f, 0.7853982f, true);
    }
}

void RunGorgeousTyphoonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_CanTakeDamage = false;
        enemy.m_InSpellcard   = true;
        enemy.m_ShowSpellName = true;
        enemy.m_BossTitle     = "Extreme Color Typhoon";
        enemy.m_BossTimer     = 0;
        enemy.m_TimerCallbackThreshold = 2160;
        enemy.m_TimerCallbackSub       = SUB_MEILING_DEATH;
        enemy.m_LifeCallbackThreshold  = -1;
        enemy.m_LifeCallbackSub        = -1;
        enemy.m_DeathCallbackSub       = SUB_MEILING_DEATH;
        ScriptUtil::DropPowerItems(enemy, ctx, 5);
        ctx.BulletCancelIntoPointItems();
    }
    if (t == 60) {
        ctx.StartLerpTo(enemy, 192.0f, 128.0f, 120);
    }
    if (t == 180) {
        enemy.m_CanTakeDamage      = true;
        enemy.m_LockedShotAngle    = PI / 2.0f;
        enemy.m_SecondaryShotAngle = -PI / 2.0f;
    }
    if (t < 180) return;

    const int rel   = t - 180;
    const int count = rel > 1200 ? 3 : 2;
    const int phase = rel % 6;
    const glm::vec2 pos = ShootPos(enemy, {0.0f, 0.0f});

    if (phase == 0) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Blue, count, 1.0f,
                               0.3f, 0.016f, enemy.m_LockedShotAngle, true);
    } else if (phase == 1) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Green, 1, 1.0f,
                               0.3f, 0.018f, enemy.m_LockedShotAngle + Util::HALF_PI, true);
    } else if (phase == 2) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Cyan, 1, 1.0f,
                               0.3f, 0.018f, enemy.m_LockedShotAngle - Util::HALF_PI, true);
    } else if (phase == 3) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Red, count, 1.0f,
                               0.3f, 0.016f, enemy.m_SecondaryShotAngle, true);
    } else if (phase == 4) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Red, 1, 1.0f, 0.3f,
                               0.018f, enemy.m_SecondaryShotAngle + PI, true);
    } else {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Red, 1, 1.0f, 0.3f,
                               0.016f, enemy.m_SecondaryShotAngle - Util::HALF_PI, true);
        enemy.m_LockedShotAngle += 0.10471976f;
        enemy.m_SecondaryShotAngle -= 0.10471976f;
    }
}

}  // namespace

void Stage3Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG3ENM.folder, Anm::STG3ENM.txt, Anm::STG3ENM.offset);
}

void Stage3Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG3ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
            ctx.anm.SetScript(enemy.m_Vm, offset + (enemy.m_SubId == 0 ? 13 : 14), offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? 2.6179938f : 0.5235988f;
            enemy.m_Speed      = 4.5f;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            break;

        case 2:
        case 3:
            ctx.anm.SetScript(enemy.m_Vm, offset + 13, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? -2.0943952f : -1.0471976f;
            enemy.m_Speed      = 4.0f;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            break;

        case 4:
        case 7:
        case 8:
        case SUB_MEILING_SUPPORT_FAIRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 15, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = enemy.m_SubId == 8 ? 2.5f : 1.5f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 5:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 1.5f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 6:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case SUB_MEILING_MIDBOSS_MAIN:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(352.0f, -96.0f);
            enemy.m_HitboxSize             = {56.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_BlocksTimeline         = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossTitle              = "Hong Meiling";
            enemy.m_BossLifeCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_TimerCallbackThreshold = 1800;
            enemy.m_TimerCallbackSub       = SUB_MEILING_MIDBOSS_ESCAPE;
            enemy.m_LifeCallbackThreshold  = 1300;
            enemy.m_LifeCallbackSub        = SUB_MEILING_MIDBOSS_SPELL_A;
            enemy.m_DeathCallbackSub       = SUB_MEILING_MIDBOSS_DEATH;
            enemy.m_BoundsMin              = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax              = Util::GameFieldToScreen(352.0f, 144.0f);
            SetMeilingBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case SUB_MEILING_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos              = Util::GameFieldToScreen(352.0f, -96.0f);
            enemy.m_HitboxSize       = {56.0f, 56.0f};
            enemy.m_IsBoss           = true;
            enemy.m_BlocksTimeline   = true;
            enemy.m_CanTakeDamage    = false;
            enemy.m_ItemDropCount    = 0;
            enemy.m_BossTitle        = "Hong Meiling";
            enemy.m_BossLifeCount    = 2;
            enemy.m_BoundsMin        = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax        = Util::GameFieldToScreen(352.0f, 144.0f);
            enemy.m_DeathCallbackSub = SUB_MEILING_DEATH;
            SetMeilingBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void Stage3Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
            RunOpeningFairy(enemy, t);
            break;

        case 2:
        case 3:
            RunSideFairy(enemy, t);
            break;

        case 4:
            RunMediumKunai(enemy, ctx, t);
            break;

        case 5:
            RunMediumRing(enemy, ctx, t);
            break;

        case 6:
            RunBlueScatter(enemy, ctx, t);
            break;

        case 7:
            RunFixedDownKunaiFairy(enemy, ctx, t);
            break;

        case 8:
            RunWhiteRandomBalls(enemy, ctx, t);
            break;

        case SUB_MEILING_MIDBOSS_MAIN:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
            if (t == 100) enemy.m_CanTakeDamage = true;
            if (t >= 130) RunMeilingMidbossPattern(enemy, ctx, t - 130);
            break;

        case SUB_MEILING_MIDBOSS_SPELL_A:
        case SUB_MEILING_MIDBOSS_SPELL_B:
            RunMeilingMidbossSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_MIDBOSS_DEATH:
            if (t == 0) {
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Life);
                ctx.TransitionToSub(enemy, SUB_MEILING_MIDBOSS_ESCAPE);
            }
            break;

        case SUB_MEILING_MIDBOSS_ESCAPE:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                enemy.m_InSpellcard   = false;
                ctx.BulletCancelIntoPointItems();
            }
            if (t == 130) {
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 190) enemy.m_Alive = false;
            break;

        case SUB_MEILING_SUPPORT_FAIRY:
            RunMeilingSupportFairy(enemy, ctx, t);
            break;

        case SUB_MEILING_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
            if (t == 120) ctx.TransitionToSub(enemy, SUB_MEILING_FIRST_NONSPELL);
            break;

        case SUB_MEILING_FIRST_NONSPELL:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 14000, 2, 1800, SUB_MEILING_RAINBOW_WIND_CHIME, SUB_MEILING_SECOND_NONSPELL);
                enemy.m_LifeCallbackThreshold = 1900;
                enemy.m_LifeCallbackSub       = SUB_MEILING_RAINBOW_WIND_CHIME;
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 50) RunMeilingFirstNonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_RAINBOW_WIND_CHIME:
            if (t == 0) {
                StartSpellPhase(enemy, ctx, "Rainbow Wind Chime", 2, 1800, SUB_MEILING_SECOND_NONSPELL);
                enemy.m_LockedShotAngle = 0.87266463f;
            }
            RunRainbowWindChimeSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_SECOND_NONSPELL:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 12000, 1, 2400, SUB_MEILING_FINAL_NONSPELL, SUB_MEILING_FINAL_NONSPELL);
                enemy.m_LifeCallbackThreshold = -1;
                enemy.m_LifeCallbackSub       = -1;
                ScriptUtil::DropPowerItems(enemy, ctx, 5);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 160) RunMeilingSecondNonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_FINAL_NONSPELL:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 16500, 0, 2400, SUB_MEILING_COLORFUL_RAIN_A, SUB_MEILING_DEATH);
                enemy.m_LifeCallbackThreshold = 3400;
                enemy.m_LifeCallbackSub       = SUB_MEILING_COLORFUL_RAIN_A;
                ScriptUtil::DropPowerItems(enemy, ctx, 5);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 160) RunMeilingFinalNonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_COLORFUL_RAIN_A:
        case SUB_MEILING_COLORFUL_RAIN_B:
            if (t == 0) {
                StartSpellPhase(enemy, ctx, "Colorful Rain", 0, 2160, SUB_MEILING_EXTREME_TYPHOON);
                enemy.m_LifeCallbackThreshold = 2000;
                enemy.m_LifeCallbackSub       = SUB_MEILING_EXTREME_TYPHOON;
            }
            RunColorfulRainSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_EXTREME_TYPHOON:
            RunGorgeousTyphoonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_DEATH:
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
