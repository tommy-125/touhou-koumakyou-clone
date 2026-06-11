#include <algorithm>
#include <cmath>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Scene/Stage4/Stage4PatternHelper.hpp"
#include "Scene/Stage4/Stage4Patterns.hpp"
#include "Util/Math.hpp"

namespace Stage4Detail {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;
using EnemyPatternUtil::RandAngle;
using EnemyPatternUtil::SpawnAimedCircleLinearStack;

void InitPatchouliEntry(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitBossEntry(
        enemy, ctx, StageUtil::LoadBossEntryConfig(StageUtil::ConfigId::BossEntry::Stage4Patchouli));
    SetPatchouliBossPoses(enemy);
}

void SpawnPatchouliLaserSet(Enemy&, EnemySubCtx& ctx, float spin, int duration, bool clearFirst) {
    if (clearFirst) ctx.lasers.ClearAll();

    const glm::vec2 laserOrigin = Util::GameFieldToScreen(192.0f, 128.0f);
    const float     angles[]    = {0.7853982f, 2.3561945f, -2.3561945f, -0.7853982f};
    for (float angle : angles) {
        ctx.lasers.SpawnAtAngle(laserOrigin, angle, 500.0f, 24.0f, 30, duration, 20, 30, 14, spin,
                                0.0f, 120, 64.0f);
    }
}

void SpawnPatchouliLaserPointBullets(EnemySubCtx& ctx, int cycle, int laserT) {
    if (cycle == 0 || laserT < 0 || laserT >= 180 || (laserT % 50) != 0) return;

    const bool      sub33       = (cycle % 2) == 0;
    const float     firstSpin   = sub33 ? -0.006829549f : 0.006829549f;
    const float     secondSpin  = -firstSpin;
    const glm::vec2 origin      = Util::GameFieldToScreen(192.0f, 128.0f);
    const float     startOffset = 64.0f;
    const float     angles[]    = {0.7853982f, 2.3561945f, -2.3561945f, -0.7853982f};

    const auto spawnAtAngle = [&](float angle) {
        const glm::vec2 pos = origin + glm::vec2{std::cos(angle), std::sin(angle)} * startOffset;
        if (sub33) {
            SpawnAimedCircleLinearStack(ctx, pos, EBulletType::Ball, EBulletColor::Red, 1, 2, 2.0f,
                                        1.2f);
        } else {
            ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, EBulletType::Ball, EBulletColor::Red,
                                         10, 2.0f, 0.0f, false);
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

void RunPatchouliFirstNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
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
    if (loopT == 180) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    if (loopT >= 180 && loopT < 360 && (loopT - 180) % 40 == 0) {
        SpawnAimedCircleLinearStack(ctx, ShootPos(enemy), EBulletType::Ball, EBulletColor::Red,
                                    std::min(26, 16 + cycle), 4, 3.5f, 1.2f);
    }
    if (loopT == 270) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 90);
}

void RunPatchouliSecondNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
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
        SpawnAimedCircleLinearStack(ctx, ShootPos(enemy), EBulletType::Ball, EBulletColor::Blue,
                                    std::min(22, 10 + cycle), 3, 4.0f, 1.5f);
    }
    if (loopT == 270) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 90);
    if (loopT == 360) {
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                     EBulletColor::Blue, 12, 1.5f, 0.0f, false);
    }
}

void RunPrincessUndine(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 250;
    if (loopT < 120 && loopT % 22 == 0) {
        ctx.bullets.SpawnFanStack(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos, EBulletType::Ball,
                                  EBulletColor::Blue, 16, 2, 3.5f, 1.2f, PI, 0.34906584f, false);
    }
    if (loopT < 120 && loopT % 10 == 0) {
        const float offset = PI / 8.0f - static_cast<float>(loopT / 10) * 0.02617994f;
        const auto  pos    = ShootPos(enemy);
        const float aim    = std::atan2(ctx.playerPos.y - pos.y, ctx.playerPos.x - pos.x);
        ctx.lasers.SpawnAtAngle(pos, aim, 96.0f, 6.0f, 0, 9999, 30, 0, 30, 0.0f, 4.0f);
        ctx.lasers.SpawnAtAngle(pos, aim + offset, 96.0f, 6.0f, 0, 9999, 30, 0, 30, 0.0f, 4.0f);
        ctx.lasers.SpawnAtAngle(pos, aim - offset, 96.0f, 6.0f, 0, 9999, 30, 0, 30, 0.0f, 4.0f);
    }
    if (loopT >= 120 && loopT < 200 && loopT % 10 == 0) {
        const float drift = static_cast<float>(loopT - 120) * 0.02f;
        ctx.bullets.SpawnFanAimed(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos,
                                  EBulletType::BigBall, EBulletColor::DarkPurple, 10, 2.5f, drift,
                                  0.22439948f, false, false);
        ctx.bullets.SpawnFanAimed(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos, EBulletType::Ball,
                                  EBulletColor::Blue, 6, 0.8f, -drift, 0.34906584f, false, false);
    }
    if (loopT == 130) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    if (loopT == 210) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

void SpawnEdgeShardRain(EnemySubCtx& ctx, EBulletColor color, bool fromLeft, int count = 1,
                        float speed = 1.2f) {
    for (int i = 0; i < count; i++) {
        const float x = fromLeft ? -12.0f : 396.0f;
        const float y = ScriptUtil::RandFloat(32.0f, 224.0f);
        const float angle =
            fromLeft ? ScriptUtil::RandFloat(0.45f, 1.15f) : ScriptUtil::RandFloat(1.99f, 2.70f);
        ctx.bullets.SpawnCircle(Util::GameFieldToScreen(x, y), EBulletType::Shard, color, 1, speed,
                                angle, false, 0.0f, 0, true);
    }
}

void RunSylphyHornAdvanced(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 126;
    if (loopT % 24 == 0) {
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Shard,
                                EBulletColor::Yellow, 15, 2.0f, RandAngle(), false, 0.0f, 0, true);
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
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int  attackT = t - 120;
    const int  loopT   = attackT % 70;
    const int  cycle   = attackT / 70;
    const auto pos     = enemy.m_Pos;

    if (loopT == 0) {
        if (cycle % 8 == 0) enemy.m_SecondaryShotAngle = 0.0f;
        SpawnAimedCircleLinearStack(ctx, pos, EBulletType::Ball, EBulletColor::Cyan, 24, 2, 2.8f,
                                    1.2f, PI);
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::BigBall,
                                  EBulletColor::DarkPurple, 10, 1.8f, enemy.m_SecondaryShotAngle,
                                  0.34906584f, false, false);
        enemy.m_SecondaryShotAngle -= ScriptUtil::RandFloat(-0.11219974f, 0.11219974f);
    }
    if (loopT == 20) {
        SpawnAimedCircleLinearStack(ctx, pos, EBulletType::Ball, EBulletColor::Green, 20, 2, 2.8f,
                                    1.2f, PI);
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
}

void RunMercuryPoison(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int  attackT = t - 120;
    const int  loopT   = attackT % 370;
    const int  cycle   = attackT / 370;
    const int  count   = std::min(28, 15 + cycle);
    const auto pos     = enemy.m_Pos;

    if (loopT < 320 && loopT % 40 == 0) {
        enemy.m_LockedShotAngle = RandAngle();
        SpawnAimedCircleLinearStack(ctx, pos, EBulletType::Ball, EBulletColor::Orange, count, 2,
                                    1.5f, 0.8f, enemy.m_LockedShotAngle, false, 0.003f, 90,
                                    0.012271847f, 90);
    }
    if (loopT < 320 && loopT % 40 == 20) {
        SpawnAimedCircleLinearStack(ctx, pos, EBulletType::Ball, EBulletColor::Cyan, count, 2, 1.5f,
                                    0.8f, enemy.m_LockedShotAngle, false, 0.003f, 90, -0.012271847f,
                                    90);
    }
    if (loopT == 320) {
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
}

void RunWaterElf(Enemy& enemy, EnemySubCtx& ctx, int t) {
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
                                EBulletColor::Red, 1, 1.2f, ScriptUtil::RandFloat(2.08f, 2.64f),
                                false, 0.0f, 0, true);
        SpawnEdgeShardRain(ctx, EBulletColor::Red, false);
    }
    if (loopT == 90) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

}  // namespace Stage4Detail
