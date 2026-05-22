#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage1/Stage1Patterns.hpp"
#include "Util/Math.hpp"

namespace Stage1Detail {
namespace {
namespace ScriptUtil = EnemyScriptUtil;

}  // namespace
void InitStage1Sub(Enemy& enemy, EnemySubCtx& ctx) {
    int offset = Anm::STG1ENM.offset;

    switch (enemy.m_SubId) {
        // Regular enemies (small/medium fairies)
        case 0:  // Small fairy: straight-down with angular drift
        case 1:  // Small fairy: single-turn variant
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 2:  // Medium fairy: stops and shoots fan
        case 3:  // Medium fairy: same movement, no shot on Normal
            ctx.anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28, 28};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = 0;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            break;

        // Mid-boss entry
        case SUB_MIDBOSS_MAIN:
            InitRumiaMidbossSub(enemy, ctx);
            break;
        case SUB_BOSS_ENTRY:
            InitRumiaBossSub(enemy, ctx);
            break;
        default:
            break;
    }
}

void RunStage1Sub(Enemy& enemy, EnemySubCtx& ctx) {
    int   t      = enemy.m_FrameTimer;
    int   offset = Anm::STG1ENM.offset;
    float dir    = enemy.m_Mirrored ? -1.0f : 1.0f;

    switch (enemy.m_SubId) {
        // Regular enemy waves
        case 0:  // Small fairy: angular drift pattern A
            if (t == 40) enemy.m_AngularVelocity = dir * -0.024543693f;
            if (t == 120) enemy.m_AngularVelocity = dir * 0.019634955f;
            if (t == 220) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 1:  // Small fairy: angular drift pattern B
            if (t == 100) enemy.m_AngularVelocity = dir * 0.019634955f;
            if (t == 200) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 2:  // Medium fairy: stops, shoots 7-way fan, re-accelerates
            if (t == 60) {
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 70) {
                glm::vec2 shootPos = enemy.m_Pos + glm::vec2{12.0f, -12.0f};
                ctx.bullets.SpawnFanAimed(shootPos, ctx.playerPos, EBulletType::RingBall,
                                          EBulletColor::Red, 7, 1.4f, 0.0f, 0.62831855f, true);
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = dir * 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 3:  // Medium fairy: same movement, no shot on Normal
            if (t == 60) {
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = dir * 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        // Mid-boss
        case SUB_MIDBOSS_MAIN:
        case SUB_MIDBOSS_ESCAPE:
        case SUB_MIDBOSS_DEATH:
            RunRumiaMidbossSub(enemy, ctx, t);
            break;
        case SUB_BOSS_ENTRY:
        case SUB_BOSS_PHASE1_INIT:
        case SUB_BOSS_PHASE1_ATTACK_A:
        case SUB_BOSS_PHASE1_ATTACK_B:
        case SUB_BOSS_PHASE1_ATTACK_C:
        case SUB_BOSS_PHASE1_ATTACK_D:
        case SUB_BOSS_PHASE1_SPELL:
        case SUB_BOSS_PHASE2_INIT:
        case SUB_BOSS_PHASE2_ATTACK_A:
        case SUB_BOSS_PHASE2_ATTACK_B:
        case SUB_BOSS_PHASE2_ATTACK_C:
        case SUB_BOSS_PHASE2_ATTACK_D:
        case SUB_BOSS_PHASE2_SPELL:
        case SUB_BOSS_DEATH:
            RunRumiaBossSub(enemy, ctx, t);
            break;
        default:
            break;
    }
}

}  // namespace Stage1Detail
