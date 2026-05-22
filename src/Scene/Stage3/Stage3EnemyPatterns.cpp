#include <cmath>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"
#include "Util/Math.hpp"

namespace Stage3Detail {
namespace ScriptUtil = EnemyScriptUtil;
using EnemyPatternUtil::AimAngleToPlayer;
using EnemyPatternUtil::SpawnRandomVarianceCircle;
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
        enemy.m_Acceleration    = 0.0f;
        enemy.m_Speed           = 0.0f;
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
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration  = 0.0f;
        enemy.m_Speed         = 0.0f;
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
                                  EBulletColor::DarkRed, 3, 1.5f, 0.0f, 0.05235988f, false, true);
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
        enemy.m_Acceleration  = 0.0f;
        enemy.m_Speed         = 0.0f;
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
        SpawnRandomVarianceCircle(ctx, enemy.m_Pos, EBulletType::Ball, EBulletColor::White, 14,
                                  2.0f, 0.3f);
        enemy.m_Angle = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
}

}  // namespace Stage3Detail