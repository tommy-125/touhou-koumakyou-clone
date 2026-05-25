#include <cmath>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage3Detail {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;
using EnemyPatternUtil::AimAngleToPlayer;
using EnemyPatternUtil::SpawnRandomVarianceCircle;

void InitStage3OpeningFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage3OpeningFairy);
    config.visual.script = enemy.m_SubId == 0 ? 13 : 14;
    config.angle         = enemy.m_Mirrored ? 2.6179938f : 0.5235988f;
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage3SideFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage3SideFairy);
    config.angle = enemy.m_Mirrored ? -2.0943952f : -1.0471976f;
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage3Script15Fairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage3Script15Fairy);
    config.speed = enemy.m_SubId == 8 ? 2.5f : 1.5f;
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage3MediumRingFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage3MediumRingFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage3BlueScatterFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage3BlueScatterFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void RunOpeningFairy(Enemy& enemy, int t) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3OpeningFairy, t);
}

void RunSideFairy(Enemy& enemy, int t) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3SideFairy, t);
}

void RunMediumKunai(Enemy& enemy, EnemySubCtx& ctx, int t) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3Script15StopFairy,
                                    t);
    if (t == 70) {
        enemy.m_LockedShotAngle = AimAngleToPlayer(enemy, ctx);
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
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3Script15StopFairy,
                                    t);
    if (t == 70) {
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
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3BlueScatterFairy,
                                    t);
    if (t == 190) {
        ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                  EBulletColor::DarkRed, 3, 1.5f, 0.0f, 0.05235988f, false, true);
    }
}

void RunFixedDownKunaiFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3Script15StopFairy,
                                    t);
    if (t == 70) {
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
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage3WhiteRandomFairy,
                                    t);
    if (t == 70) {
        SpawnRandomVarianceCircle(ctx, enemy.m_Pos, EBulletType::Ball, EBulletColor::White, 14,
                                  2.0f, 0.3f);
        enemy.m_Angle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
}

}  // namespace Stage3Detail
