#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage2/Stage2Patterns.hpp"
#include "Util/Math.hpp"

namespace Stage2Detail {
namespace ScriptUtil = EnemyScriptUtil;
using EnemyPatternUtil::AimAngle;
using EnemyPatternUtil::SpawnRandomVarianceCircle;

void InitStage2Sub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG2ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: {
            static constexpr float angles[5]  = {0.7853982f, 1.1780972f, Util::HALF_PI, 1.9634954f,
                                                 2.3561945f};
            const bool             leftSprite = (enemy.m_SubId % 2) == 0;
            ctx.anm.SetScript(enemy.m_Vm, offset + (leftSprite ? 13 : 14), offset);
            enemy.m_HitboxSize   = {28.0f, 28.0f};
            enemy.m_Angle        = angles[enemy.m_SubId];
            enemy.m_Speed        = 3.0f;
            enemy.m_Acceleration = -0.015f;
            enemy.m_ItemDrop     = -1;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            if (enemy.m_SubId == 0 || enemy.m_SubId == 2 || enemy.m_SubId == 4) {
                enemy.m_DeathCallbackSub = 5;
            }
            break;
        }

        case 6:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = AimAngle(enemy.m_Pos, ctx.playerPos);
            enemy.m_Speed      = 2.4f;
            enemy.m_ItemDrop   = -1;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 7:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {16.0f, 16.0f};
            enemy.m_Angle      = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
            enemy.m_Speed      = 3.0f;
            enemy.m_ItemDrop   = -1;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 8:
        case 9:
        case 10:
        case 11:
            ctx.anm.SetScript(enemy.m_Vm, offset + enemy.m_SubId, offset);
            enemy.m_HitboxSize      = {22.0f, 22.0f};
            enemy.m_Angle           = ScriptUtil::RandFloat(0.7853982f, 2.3561945f);
            enemy.m_Speed           = 5.0f;
            enemy.m_ItemDrop        = -1;
            enemy.m_RotateWithAngle = true;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 12:
        case 13:
            ctx.anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            break;

        case SUB_DAIYOUSEI_MAIN:
            InitDaiyouseiSub(enemy, ctx);
            break;
        case SUB_CIRNO_ENTRY:
            InitCirnoSub(enemy, ctx);
            break;
        default:
            break;
    }
}

void RunStage2Sub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t      = enemy.m_FrameTimer;
    const int offset = Anm::STG2ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            if (t == 180 && (enemy.m_SubId == 0 || enemy.m_SubId == 2 || enemy.m_SubId == 4)) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Kunai,
                                             EBulletColor::Lime, 4, 2.0f, 0.0f, false, 0.0f, {},
                                             true);
            }
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 5:
            if (t == 0) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                             EBulletColor::Red, 4, 1.2f, PI * 0.25f, false, 0.0f,
                                             {}, true);
                enemy.m_Alive = false;
            }
            break;

        case 6:
            if (t == 180) enemy.m_AngularVelocity = -0.024543693f;
            if (t == 280) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 12:
        case 13:
            if (t == 60) {
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 70 && enemy.m_SubId == 12) {
                ctx.bullets.SpawnFanStack(enemy.m_Pos + glm::vec2{12.0f, -12.0f}, ctx.playerPos,
                                          EBulletType::RingBall, EBulletColor::DarkRed, 7, 2, 1.4f,
                                          0.8f, 0.0f, 0.62831855f);
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case SUB_DAIYOUSEI_MAIN:
        case SUB_DAIYOUSEI_DEATH:
        case SUB_DAIYOUSEI_ESCAPE:
            RunDaiyouseiSub(enemy, ctx, t);
            break;

        case SUB_CIRNO_ENTRY:
        case SUB_CIRNO_NONSPELL_INIT:
        case SUB_CIRNO_NONSPELL_ATTACK_A:
        case SUB_CIRNO_NONSPELL_ATTACK_B:
        case SUB_CIRNO_ICICLE_FALL:
        case SUB_CIRNO_PHASE2_INIT:
        case SUB_CIRNO_PREFREEZE_ATTACK_A:
        case SUB_CIRNO_PREFREEZE_ATTACK_B:
        case SUB_CIRNO_PERFECT_FREEZE:
        case SUB_CIRNO_DIAMOND_BLIZZARD:
        case SUB_CIRNO_DEATH:
            RunCirnoSub(enemy, ctx, t);
            break;
        default:
            break;
    }
}

}  // namespace Stage2Detail
