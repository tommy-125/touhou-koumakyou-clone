#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/Stage6/Stage6PatternCommon.hpp"
#include "Util/Math.hpp"

namespace Stage6Detail {
using EnemyPatternUtil::SpawnRandomArc;
void RunArcFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool red, bool highArc) {
    if (t == 0) {
        enemy.m_Angle = MirrorAngle(highArc ? 0.5235988f : -1.0471976f, enemy.m_Mirrored);
        enemy.m_Speed = highArc ? 4.5f : 4.0f;
    }
    if (t == 30)
        enemy.m_AngularVelocity =
            (enemy.m_Mirrored ? -1.0f : 1.0f) * (highArc ? -0.06544985f : 0.034906585f);
    if (t == (highArc ? 115 : 90)) enemy.m_AngularVelocity = 0.0f;
    if (t == 80) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        if (red) {
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Red, 9, 1.0f, 2.0f, -PI, PI);
        } else {
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Blue, 6, 1.6f, 1.6f, -PI, PI);
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Blue, 6, 1.0f, 1.0f, -PI, PI);
        }
    }
}

void RunBurstFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool top) {
    if (t == 0) {
        enemy.m_Angle = top ? Util::HALF_PI : (enemy.m_Mirrored ? PI : 0.0f);
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) {
        enemy.m_Acceleration = -0.06666667f;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Rice,
                                     EBulletColor::Blue, 60, 1.6f);
    }
    if (t == 42) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Angle        = RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed        = 1.8f;
    }
}
}  // namespace Stage6Detail