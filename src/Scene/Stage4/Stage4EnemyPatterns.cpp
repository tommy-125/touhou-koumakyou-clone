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

void InitStage4BookFairy(Enemy& enemy, EnemySubCtx& ctx) {
    const StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4BookFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage4SideBookFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4BookFairy);
    config.angle = enemy.m_Mirrored ? PI : 0.0f;
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage4BurstFamiliar(Enemy& enemy, EnemySubCtx& ctx) {
    const StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4BurstFamiliar);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage4SmallFairyEnemy(Enemy& enemy, EnemySubCtx& ctx) {
    const StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4SmallFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage4RingCaster(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4RingCaster);
    config.angle = enemy.m_SubId == 13 ? 0.0f : Util::HALF_PI;
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage4DropProxy(Enemy& enemy, EnemySubCtx& ctx) {
    const StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4DropProxy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage4CrystalFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config =
        StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage4CrystalFairy);
    config.visual.script = enemy.m_SubId == 16 ? 14 : 13;
    StageUtil::InitEnemy(enemy, ctx, config);
}

void SpawnAimedRiceFan(Enemy& enemy, EnemySubCtx& ctx, int t, int count, float speed, float spread,
                       int stacks = 2, float innerSpeed = -1.0f) {
    if (t < 0) return;
    ctx.bullets.SpawnFanStack(
        enemy.m_Pos, ctx.playerPos, EBulletType::Rice, EBulletColor::Blue, count, stacks, speed,
        innerSpeed >= 0.0f ? innerSpeed : std::max(0.4f, speed - 0.8f), 0.0f, spread, true);
}

void RunStage4SmallFairy(Enemy& enemy, EnemySubCtx& ctx, int t) {
    StageUtil::ConfigId::MovementId profile = StageUtil::ConfigId::Movement::Stage4SmallDown;
    switch (enemy.m_SubId) {
        case 2:
        case 4:
            profile = StageUtil::ConfigId::Movement::Stage4SmallStraightSlow;
            break;
        case 6:
            profile = StageUtil::ConfigId::Movement::Stage4SmallStraightFast;
            break;
        case 9:
            profile = StageUtil::ConfigId::Movement::Stage4SmallStraightNoTurn;
            break;
        case 3:
            profile = StageUtil::ConfigId::Movement::Stage4SmallDiagonalSlow;
            break;
        case 5:
            profile = StageUtil::ConfigId::Movement::Stage4SmallDiagonalFast;
            break;
        case 7:
            profile = StageUtil::ConfigId::Movement::Stage4SmallDiagonalShort;
            break;
        default:
            break;
    }
    StageUtil::ApplyMovementProfile(enemy, profile, t);

    switch (enemy.m_SubId) {
        case 2:
            if (t == 50) SpawnAimedRiceFan(enemy, ctx, t, 3, 1.8f, 0.15707964f, 2, 1.0f);
            break;
        case 3:
            if (t == 50 || t == 100) {
                SpawnAimedRiceFan(enemy, ctx, t, 3, 1.8f, 0.15707964f, 2, 1.0f);
            }
            break;
        case 4:
            if (t == 50) SpawnAimedRiceFan(enemy, ctx, t, 1, 1.5f, 0.17453292f, 3, 0.7f);
            break;
        case 5:
            if (t == 50 || t == 128) {
                SpawnAimedRiceFan(enemy, ctx, t, 1, 1.5f, 0.17453292f, 3, 0.7f);
            }
            break;
        case 8:
        case 9:
            if (t == 50) SpawnAimedRiceFan(enemy, ctx, t, 3, 2.0f, 0.17453292f, 2, 1.2f);
            break;
        default:
            break;
    }
}

void RunBookFairy(Enemy& enemy, EnemySubCtx& ctx, int t, int burstSub) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage4BookFairy, t);
    if (t == 70) {
        SpawnAtEnemyFieldPos(enemy, ctx, burstSub, 1000, 10,
                             static_cast<int>(ItemType::PowerSmall));
        enemy.m_Angle        = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed        = 1.8f;
    }
}

void RunBurstFamiliar(Enemy& enemy, EnemySubCtx& ctx, int t, bool laser, bool denseKunai) {
    enemy.m_CanTakeDamage = false;
    const int interval    = denseKunai ? 2 : 6;
    const int volleys     = denseKunai ? 36 : 12;
    if (!laser && t >= 30 && t < 30 + interval * volleys && (t - 30) % interval == 0) {
        const glm::vec2 pos = enemy.m_Pos + glm::vec2{ScriptUtil::RandFloat(-40.0f, 40.0f),
                                                      ScriptUtil::RandFloat(-40.0f, 40.0f)};
        if (denseKunai) {
            ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Kunai, EBulletColor::Blue, 4,
                                      2.0f, 0.0f, 0.09817477f, false, true);
        } else {
            ctx.bullets.SpawnCircle(pos, EBulletType::Kunai, EBulletColor::Blue, 4,
                                    ScriptUtil::RandFloat(1.0f, 2.0f), RandAngle(), false, 0.0f, 0,
                                    true);
        }
    }
    if (t == 120 && laser) {
        ctx.lasers.SpawnAtAngle(enemy.m_Pos, Util::HALF_PI, 500.0f, 32.0f, 90, 120, 16, 70, 14);
    }
    if (t >= (laser ? 440 : 156)) enemy.m_Alive = false;
}

void RunRingCaster(Enemy& enemy, EnemySubCtx& ctx, int t, bool sideEntry) {
    StageUtil::ApplyMovementProfile(enemy, StageUtil::ConfigId::Movement::Stage4RingCaster, t);
    if (t >= 70 && t < 70 + 64 * 8 && (t - 70) % 8 == 0) {
        const int   volley = (t - 70) / 8;
        const float sign   = enemy.m_Mirrored ? -1.0f : 1.0f;
        ctx.bullets.SpawnCircleStack(enemy.m_Pos, EBulletType::Rice, EBulletColor::Blue, 3, 2,
                                     sideEntry ? 2.1f : 2.6f, sideEntry ? 1.4f : 1.8f,
                                     RandAngle() + sign * volley * 0.24166097f, false, true);
    }
}

void RunCrystalFairy(Enemy& enemy, EnemySubCtx& ctx, int t, int maxVolleys, bool green,
                     bool largeBlue) {
    if (t == 30) enemy.m_CanTakeDamage = true;
    if (t >= 30 && t < 30 + maxVolleys * 50 && (t - 30) % 50 == 0) {
        if (green) {
            const float base = RandAngle();
            ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                         EBulletColor::Green, 11, 1.2f, base, false, 0.0f, 0, {},
                                         false);
            ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
                                         EBulletColor::Green, 11, 1.2f, base + 0.28559932f, false,
                                         0.0f, 0, {}, false);
        } else {
            ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
                                         EBulletColor::Blue, largeBlue ? 24 : 16,
                                         largeBlue ? 2.4f : 1.4f, 0.0f, false, 0.0f, 0, {}, false);
        }
    }
    if (t >= 30 + maxVolleys * 50 + 30) enemy.m_Alive = false;
}

}  // namespace Stage4Detail
