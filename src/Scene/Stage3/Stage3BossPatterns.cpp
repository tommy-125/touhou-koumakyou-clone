#include <algorithm>
#include <cmath>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage3/Stage3PatternCommon.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage3Detail {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;
using EnemyPatternUtil::AimAngleToPlayer;

void InitMeilingBossEntry(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitBossEntry(
        enemy, ctx,
        StageUtil::LoadBossEntryConfig(StageUtil::ConfigId::BossEntry::Stage3MeilingBoss));
    enemy.m_DeathCallbackSub = SUB_MEILING_DEATH;
    SetMeilingBossPoses(enemy);
}

void RunMeilingFirstNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 50) % 272;
    const int cycle = (t - 50) / 272;

    if (loopT == 0) {
        enemy.m_LockedShotAngle = ScriptUtil::RandFloat(-PI, PI);
    }

    if (loopT < 30 * 4 && loopT % 4 == 0) {
        const int   burst     = loopT / 4;
        const int   count     = std::clamp(5 + cycle, 5, 10);
        const float turnSign  = (cycle % 2 == 0) ? 1.0f : -1.0f;
        const float speed     = 2.0f + burst * 0.05f;
        const float aimOffset = enemy.m_LockedShotAngle + turnSign * 0.09817477f * burst;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Shard,
                                     EBulletColor::Red, count, speed, aimOffset, false, 0.0f, 0, {},
                                     true);
    }
    if (loopT == 120) {
        ScriptUtil::StartRandomMove(enemy, ctx, 5.0f, 40);
    }
    if (loopT >= 120 && loopT < 152 && loopT % 2 == 0) {
        const int   burst     = (loopT - 120) / 2;
        const int   count     = std::clamp(5 + cycle, 5, 10);
        const float turnSign  = (cycle % 2 == 0) ? 1.0f : -1.0f;
        const float driftStep = 0.03926991f + 0.06544985f * burst;
        const float speed     = 3.5f + burst * 0.06f;
        const float aimOffset = enemy.m_LockedShotAngle + turnSign * driftStep * burst;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Shard,
                                     EBulletColor::Red, count, speed, aimOffset, false, 0.0f, 0, {},
                                     true);
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
        ctx.bullets.SpawnCircle(pos, EBulletType::Shard, ring.color, ring.count, 2.6f, angle, false,
                                0.0f, 0, true);
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
        {{96.0f, 500}, {288.0f, 2000}},
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
            enemy.m_LockedShotAngle = AimAngleToPlayer(enemy, ctx);
            enemy.m_Acceleration    = 0.0f;
            enemy.m_Speed           = 0.0f;
        }
        if (t >= start && t < start + 60 && (t - start) % 2 == 0) {
            const int   step  = (t - start) / 2;
            const float speed = 1.6f + step * 0.18f;
            ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Kunai, EBulletColor::Blue, 1, speed,
                                    enemy.m_LockedShotAngle, false, 0.0f, 0, true);
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
                                     loopT == 70 ? EBulletColor::Blue : EBulletColor::DarkBlue, 16,
                                     2, 3.0f, 2.0f, ScriptUtil::RandFloat(-PI, PI), false, false);
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
        const bool  blueBurst = loopT == 0;
        BulletCurve curve;
        curve.at       = 60;
        curve.times    = 1;
        curve.angle    = blueBurst ? -2.8972466f : 2.8972466f;
        curve.speed    = 3.0f;
        curve.relative = true;

        const EBulletColor color = blueBurst ? EBulletColor::Blue : EBulletColor::Red;
        for (int i = 0; i < 6; i++) {
            ctx.bullets.SpawnCircle(ShootPos(enemy), EBulletType::Kunai, color, 24, 2.0f + i * 0.6f,
                                    0.0f, false, 0.0f, 0, true, {0.0f, 0.0f}, 0, 0, 1.0f, curve);
        }
    }
    if (loopT >= 22 && loopT < 102 && loopT % 2 == 0) {
        SpawnRandomDownAccel(ShootPos(enemy), ctx, EBulletType::Shard, EBulletColor::Blue, 1, 1.0f,
                             0.3f, true);
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
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Red, 1, 1.0f, 0.3f,
                               0.024f, PI, true);
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Purple, 1, 1.0f, 0.3f,
                               0.024f, 0.0f, true);
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Yellow, 1, 1.0f, 0.3f,
                               0.024f, 2.3561945f, true);
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Orange, 1, 1.0f, 0.3f,
                               0.024f, 0.7853982f, true);
    }
}

void RunGorgeousTyphoonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_CanTakeDamage          = false;
        enemy.m_InSpellcard            = true;
        enemy.m_ShowSpellName          = true;
        enemy.m_BossTitle              = "Extreme Color Typhoon";
        enemy.m_BossTimer              = 0;
        enemy.m_TimerCallbackThreshold = 2160;
        enemy.m_TimerCallbackSub       = SUB_MEILING_DEATH;
        enemy.m_LifeCallbackThreshold  = -1;
        enemy.m_LifeCallbackSub        = -1;
        enemy.m_DeathCallbackSub       = SUB_MEILING_DEATH;
        StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
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

    const int       rel   = t - 180;
    const int       count = rel > 1200 ? 3 : 2;
    const int       phase = rel % 6;
    const glm::vec2 pos   = ShootPos(enemy, {0.0f, 0.0f});

    if (phase == 0) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Blue, count, 1.0f, 0.3f,
                               0.016f, enemy.m_LockedShotAngle, true);
    } else if (phase == 1) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Green, 1, 1.0f, 0.3f,
                               0.018f, enemy.m_LockedShotAngle + Util::HALF_PI, true);
    } else if (phase == 2) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Cyan, 1, 1.0f, 0.3f,
                               0.018f, enemy.m_LockedShotAngle - Util::HALF_PI, true);
    } else if (phase == 3) {
        SpawnRandomVectorAccel(pos, ctx, EBulletType::Shard, EBulletColor::Red, count, 1.0f, 0.3f,
                               0.016f, enemy.m_SecondaryShotAngle, true);
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

}  // namespace Stage3Detail
