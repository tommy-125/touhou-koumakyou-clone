#include "Anm/AnmDefs.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/Stage6/Stage6PatternHelper.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage6Detail {
namespace StageUtil = StageScriptUtil;
using EnemyPatternUtil::SpawnRandomArc;
void InitStage6ArcFairyLeft(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitEnemy(
        enemy, ctx,
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage6ArcFairyLeft));
}

void InitStage6ArcFairyRight(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitEnemy(
        enemy, ctx,
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage6ArcFairyRight));
}

void InitStage6BurstFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitEnemy(
        enemy, ctx,
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage6BurstFairy));
    StageUtil::SetDropCallback(enemy, SUB_FAIRY_DROP_8);
}

void InitStage6FairyDropProxy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitDropProxy(enemy, ctx, Anm::STG6ENM.offset, 11);
}

void RunArcFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool red, bool highArc) {
    StageScriptUtil::ApplyMovementProfile(
        enemy,
        highArc ? StageScriptUtil::ConfigId::Movement::Stage6ArcFairyHigh
                : StageScriptUtil::ConfigId::Movement::Stage6ArcFairyLow,
        t);
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
    StageScriptUtil::ApplyMovementProfile(
        enemy,
        top ? StageScriptUtil::ConfigId::Movement::Stage6BurstFairyTop
            : StageScriptUtil::ConfigId::Movement::Stage6BurstFairySide,
        t);
    if (t == 40) {
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
