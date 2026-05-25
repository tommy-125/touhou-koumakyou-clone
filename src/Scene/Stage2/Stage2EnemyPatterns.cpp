#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage2/Stage2Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage2Detail {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;
using EnemyPatternUtil::AimAngle;
using EnemyPatternUtil::SpawnRandomVarianceCircle;

void InitStage2AngledFairy(Enemy& enemy, EnemySubCtx& ctx) {
    static constexpr float     angles[5] = {0.7853982f, 1.1780972f, Util::HALF_PI, 1.9634954f,
                                            2.3561945f};
    StageUtil::EnemyInitConfig config    = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage2AngledFairy);
    config.visual.script                 = (enemy.m_SubId % 2) == 0 ? 13 : 14;
    config.angle                         = angles[enemy.m_SubId];
    if (enemy.m_SubId == 0 || enemy.m_SubId == 2 || enemy.m_SubId == 4) {
        config.deathCallbackSub = 5;
    }
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage2AimedFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage2AimedFairy);
    config.angle                      = AimAngle(enemy.m_Pos, ctx.playerPos);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage2RandomFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage2RandomFairy);
    config.angle                      = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage2ShardFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage2ShardFairy);
    config.visual.script              = enemy.m_SubId;
    config.angle                      = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage2MediumFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage2MediumFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void RunStage2AngledFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 180 && (enemy.m_SubId == 0 || enemy.m_SubId == 2 || enemy.m_SubId == 4)) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Kunai,
                                     EBulletColor::Lime, 4, 2.0f, 0.0f, false, 0.0f, {}, true);
    }
    if (t >= 10000) enemy.m_Alive = false;
}

void RunStage2DeathBurst(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                     EBulletColor::Red, 4, 1.2f, PI * 0.25f, false, 0.0f, {}, true);
        enemy.m_Alive = false;
    }
}

void RunStage2AimedFairy(Enemy& enemy, int t) {
    if (t == 180) enemy.m_AngularVelocity = -0.024543693f;
    if (t == 280) enemy.m_AngularVelocity = 0.0f;
    if (t >= 10000) enemy.m_Alive = false;
}

void RunStage2TimedDespawn(Enemy& enemy, int t) {
    if (t >= 10000) enemy.m_Alive = false;
}

void RunStage2MediumFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int offset = Anm::STG2ENM.offset;
    if (t == 60) {
        ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
        enemy.m_Speed = 0.0f;
    }
    if (t == 70 && enemy.m_SubId == 12) {
        ctx.bullets.SpawnFanStack(enemy.m_Pos + glm::vec2{12.0f, -12.0f}, ctx.playerPos,
                                  EBulletType::RingBall, EBulletColor::DarkRed, 7, 2, 1.4f, 0.8f,
                                  0.0f, 0.62831855f);
    }
    if (t == 130) {
        enemy.m_Acceleration    = 0.05f;
        enemy.m_AngularVelocity = 0.05235988f;
    }
    if (t == 190) enemy.m_AngularVelocity = 0.0f;
    if (t >= 10000) enemy.m_Alive = false;
}

}  // namespace Stage2Detail
