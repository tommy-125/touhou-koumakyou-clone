#include <algorithm>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage2/Stage2Patterns.hpp"
#include "Util/Math.hpp"

namespace Stage2Detail {
namespace ScriptUtil = EnemyScriptUtil;

void RunDaiyouseiMove(Enemy& enemy, EnemySubCtx& ctx, int local) {
    const int offset = Anm::STG2ENM.offset;
    if (local == 0) {
        const float currentX  = enemy.m_Pos.x - Util::FIELD_OFFSET_X;
        const float targetX   = ScriptUtil::RandFloat(32.0f, 352.0f);
        enemy.m_CanTakeDamage = false;
        enemy.m_LerpTarget.x  = targetX;
        ctx.anm.SetScript(enemy.m_Vm, offset + (currentX >= 192.0f ? 66 : 67), offset);
    } else if (local == 40) {
        const float targetX = enemy.m_LerpTarget.x;
        ctx.anm.SetScript(enemy.m_Vm, offset + (targetX >= 192.0f ? 68 : 69), offset);
        enemy.m_Pos           = Util::GameFieldToScreen(targetX, 96.0f);
        enemy.m_IsLerping     = false;
        enemy.m_CanTakeDamage = true;
    }
}

int DaiyouseiAttackDuration(int attackType) {
    return attackType == 2 ? 200 : 88;
}

void RunDaiyouseiAttack(Enemy& enemy, EnemySubCtx& ctx, int attackType, int local) {
    if (attackType == 0 || attackType == 1) {
        if (local < 40 || local >= 88) return;

        const int   step   = local - 40;
        const float speed1 = 1.5f + static_cast<float>(step) * 0.05f;
        const float speed2 = 1.4f - static_cast<float>(step) * 0.02f;
        const float direction =
            Util::HALF_PI + static_cast<float>(step) * (attackType == 0 ? 0.1308997f : -0.1308997f);
        ctx.bullets.SpawnCircleStack(enemy.m_Pos, EBulletType::Kunai,
                                     attackType == 0 ? EBulletColor::Green : EBulletColor::Red, 1,
                                     2, speed1, speed2, direction, false, true);
        return;
    }

    if (local < 40 || local >= 200 || (local - 40) % 10 != 0) return;

    ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Shard, EBulletColor::White,
                              3, 2, 4.0f, 2.0f, 0.0f, 0.5235988f, true);
    ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Shard, EBulletColor::Blue, 3,
                              2.5f, 0.0f, 0.5235988f, false, true);
}

void RunDaiyouseiPattern(Enemy& enemy, EnemySubCtx& ctx, int frame) {
    int cursor = 0;
    for (int cycle = 0; cycle < 8; cycle++) {
        const int attackType     = cycle % 3;
        const int attackDuration = DaiyouseiAttackDuration(attackType);

        if (frame >= cursor && frame < cursor + attackDuration) {
            RunDaiyouseiAttack(enemy, ctx, attackType, frame - cursor);
            return;
        }
        cursor += attackDuration;

        if (frame >= cursor && frame < cursor + 80) {
            RunDaiyouseiMove(enemy, ctx, frame - cursor);
            return;
        }
        cursor += 80;

        if (frame >= cursor && frame < cursor + 80) return;
        cursor += 80;
    }
}

void InitDaiyouseiSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG2ENM.offset;
    switch (enemy.m_SubId) {
        case SUB_DAIYOUSEI_MAIN:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(192.0f, -32.0f);
            enemy.m_HitboxSize             = {45.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_BossTitle              = "Daiyousei";
            enemy.m_BossLifeCount          = 0;
            enemy.m_TimerCallbackThreshold = 1920;
            enemy.m_TimerCallbackSub       = SUB_DAIYOUSEI_ESCAPE;
            enemy.m_DeathCallbackSub       = SUB_DAIYOUSEI_DEATH;
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void RunDaiyouseiSub(Enemy& enemy, EnemySubCtx& ctx, int t) {
    switch (enemy.m_SubId) {
        case SUB_DAIYOUSEI_MAIN: {
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 60);
            if (t == 60) enemy.m_CanTakeDamage = true;

            if (t >= 130) RunDaiyouseiPattern(enemy, ctx, t - 130);
            break;
        }

        case SUB_DAIYOUSEI_DEATH:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                enemy.m_Alive = false;
            }
            break;

        case SUB_DAIYOUSEI_ESCAPE:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 60) enemy.m_Alive = false;
            break;

        default:
            break;
    }
}

}  // namespace Stage2Detail
