#include <algorithm>
#include <cmath>

#include "Anm/AnmDefs.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/Stage5/Stage5PatternCommon.hpp"
#include "Util/Math.hpp"

namespace Stage5Detail {
void InitStage5TopMaid(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitEnemy(
        enemy, ctx,
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage5TopMaid));
    if (enemy.m_SubId == 2) {
        StageUtil::SetDropCallback(enemy, SUB_MAID_DROP_4);
    } else if (enemy.m_SubId >= 3 && enemy.m_SubId <= 5) {
        StageUtil::SetDropCallback(enemy, SUB_MAID_DROP_6);
    }
}

void InitStage5SideMaid(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitEnemy(
        enemy, ctx,
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage5SideMaid));
}

void InitStage5RingMaid(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitEnemy(
        enemy, ctx,
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage5RingMaid));
}

void InitStage5MaidDropProxy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitDropProxy(enemy, ctx, Anm::STG5ENM.offset, 16);
}

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
    StageUtil::ApplyMovementProfile(enemy,
                                    dense ? StageUtil::ConfigId::Movement::Stage5TopMaidDense
                                          : StageUtil::ConfigId::Movement::Stage5TopMaid,
                                    t);

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

}

void RunSideMaid(Enemy& enemy, EnemySubCtx& ctx, int t, int variant) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage5SideMaid, t);
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
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage5RingMaid, t);
    if (t >= 70 && t < 190 && (t - 70) % 3 == 0) {
        const float angle = ScriptUtil::RandFloat(-0.3926991f, 3.5342917f);
        const float speed = ScriptUtil::RandFloat(1.0f, 2.0f);
        const auto  aim   = ShootPos(enemy) + glm::vec2{std::cos(angle), std::sin(angle)};
        ctx.bullets.SpawnFanAimed(ShootPos(enemy), aim, EBulletType::RingBall, EBulletColor::Blue,
                                  5, speed, 0.0f, 0.03926991f);
    }
}
}  // namespace Stage5Detail
