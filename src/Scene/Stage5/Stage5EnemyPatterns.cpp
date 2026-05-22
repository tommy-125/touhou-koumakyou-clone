#include <algorithm>
#include <cmath>

#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/Stage5/Stage5PatternCommon.hpp"
#include "Util/Math.hpp"

namespace Stage5Detail {
void SpawnRandomArc(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                    int count, int stacks, float speed1, float speed2, float minAngle,
                    float maxAngle, bool rotate = false) {
    for (int s = 0; s < stacks; s++) {
        for (int i = 0; i < count; i++) {
            const float speed =
                ScriptUtil::RandFloat(std::min(speed1, speed2), std::max(speed1, speed2));
            ctx.bullets.SpawnCircle(pos, type, color, 1, speed,
                                    ScriptUtil::RandFloat(minAngle, maxAngle), false, 0.0f, 0,
                                    rotate);
        }
    }
}

void RunTopMaid(Enemy& enemy, EnemySubCtx& ctx, int t, EBulletColor color, bool dense,
                bool randomFan) {
    if (t == 0) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) enemy.m_Acceleration = -0.06666667f;
    if (t == 70) enemy.m_Acceleration = 0.0f;

    if (dense) {
        if (t >= 70 && t < 90 && (t - 70) % 4 == 0) {
            const int   step  = (t - 70) / 4;
            const float speed = 1.5f + step * 0.55f;
            ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball, color,
                                         24, speed, 0.0f, false);
        }
    } else if (randomFan) {
        if (t >= 70 && t < 90 && (t - 70) % 10 == 0) {
            const int   step  = (t - 70) / 5;
            const float speed = 1.5f + step * 0.2f;
            ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball, color,
                                         24, speed, 0.1308997f, false);
        } else if (t >= 75 && t < 90 && (t - 75) % 10 == 0) {
            const int   step  = (t - 70) / 5;
            const float speed = 1.5f + step * 0.2f;
            ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball, color,
                                         24, speed, 0.0f, false);
        } else if (t >= 90 && t < 110 && (t - 90) % 5 == 0) {
            const int   step  = (t - 90) / 5;
            const float speed = 2.3f + step * 0.25f;
            ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball, color,
                                         24, speed, 0.0f, false);
        }
    } else {
        if (t >= 70 && t < 110 && (t - 70) % 5 == 0) {
            const int   step  = (t - 70) / 5;
            const float speed = 1.5f + step * 0.38f;
            ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball, color,
                                         30, speed, 0.0f, false);
        }
    }

    if (t == (dense ? 90 : 110)) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 1.8f;
    }
}

void RunSideMaid(Enemy& enemy, EnemySubCtx& ctx, int t, int variant) {
    if (t == 0) {
        enemy.m_Angle = enemy.m_Mirrored ? PI : 0.0f;
        enemy.m_Speed = 2.5f;
    }
    const int interval = variant == 11 ? 10 : (variant == 10 ? 30 : (variant == 9 ? 40 : 60));
    if (t % interval != 0) return;

    const auto pos = ShootPos(enemy, {0.0f, 0.0f});
    if (variant == 10) {
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Kunai, EBulletColor::Red, 4,
                                  1.5f, 0.0f, 0.2617994f, false, true);
    } else if (variant == 11) {
        SpawnRandomArc(pos, ctx, EBulletType::Ball, EBulletColor::Red, 4, 1, 1.5f, 0.8f, -PI, PI);
    } else if (variant == 1) {
        SpawnRandomArc(pos, ctx, EBulletType::Rice, EBulletColor::Blue, 3, 2, 1.8f, 0.8f, 0.0f, PI,
                       true);
    } else {
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Pellet, EBulletColor::Blue, 6,
                                  1.5f, 0.0f, 0.19634955f);
    }
}

void RunRingMaid(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) enemy.m_Acceleration = -0.06666667f;
    if (t == 70) enemy.m_Acceleration = 0.0f;
    if (t >= 70 && t < 190 && (t - 70) % 3 == 0) {
        const float angle = ScriptUtil::RandFloat(-0.3926991f, 3.5342917f);
        const float speed = ScriptUtil::RandFloat(1.0f, 2.0f);
        const auto  aim   = ShootPos(enemy) + glm::vec2{std::cos(angle), std::sin(angle)};
        ctx.bullets.SpawnFanAimed(ShootPos(enemy), aim, EBulletType::RingBall, EBulletColor::Blue,
                                  5, speed, 0.0f, 0.03926991f);
    }
    if (t == 190) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 1.8f;
    }
}
}  // namespace Stage5Detail