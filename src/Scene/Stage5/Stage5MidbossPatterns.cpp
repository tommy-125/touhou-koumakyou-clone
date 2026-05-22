#include <algorithm>
#include <cstdlib>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/Stage5/Stage5PatternCommon.hpp"
#include "Util/Math.hpp"

namespace Stage5Detail {
namespace ScriptUtil = EnemyScriptUtil;
using EnemyPatternUtil::SpawnAimedCircleStack;
using EnemyPatternUtil::SpawnFanAbsolute;

void SetBossFieldPosition(Enemy& enemy, float x, float y) {
    enemy.m_Pos       = Util::GameFieldToScreen(x, y);
    enemy.m_IsLerping = false;
    enemy.m_Speed     = 0.0f;
}

void SpawnMidbossDaggerSweep(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle) {
    SpawnFanAbsolute(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f, 3.2f,
                     baseAngle, 0.28559932f, true);
}

void SpawnMidbossKunaiSweep(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle) {
    SpawnFanAbsolute(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 4, 2, 2.0f, 1.0f, baseAngle,
                     0.044879895f, true);
}

void SpawnMisdirectionOpeningKnives(EnemySubCtx& ctx, glm::vec2 pos) {
    SpawnAimedCircleStack(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 24, 3, 3.0f, 1.2f, 0.0f,
                          true);
}

void SpawnMisdirectionDaggerVolley(EnemySubCtx& ctx, glm::vec2 pos) {
    ctx.bullets.SpawnFanStack(pos, ctx.playerPos, EBulletType::Dagger, EBulletColor::DarkPurple, 11,
                              4, 4.5f, 1.2f, 0.0f, 0.20943952f, true);
}

void RunMisdirection(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const bool active = BeginSakuyaSpellAt(enemy, ctx, t, "Illusion Sign \"Misdirection\"", 0,
                                           SUB_SAKUYA_MIDBOSS_EXIT, {192.0f, 112.0f});
    if (t == 0) {
        enemy.m_DeathCallbackSub = SUB_SAKUYA_MIDBOSS_DEATH;
        enemy.m_LockedShotAngle  = (std::rand() & 1) ? 1.0f : 0.0f;
    }
    if (!active) return;

    const int  loopT     = (t - 120) % 248;
    const int  cycle     = (t - 120) / 248;
    const bool leftFirst = (((cycle + static_cast<int>(enemy.m_LockedShotAngle)) & 1) == 0);

    if (loopT == 0) {
        ctx.StartLerpTo(enemy, leftFirst ? 96.0f : 288.0f, 144.0f, 40);
    }
    if (loopT < 30 && loopT % 6 == 0) {
        SpawnMisdirectionOpeningKnives(ctx, ShootPos(enemy, {0.0f, 0.0f}));
    }
    if (loopT == 60) {
        SetBossFieldPosition(enemy, leftFirst ? 288.0f : 96.0f, 96.0f);
    }
    if (loopT == 80 || loopT == 98 || loopT == 116) {
        SpawnMisdirectionDaggerVolley(ctx, ShootPos(enemy, {0.0f, 0.0f}));
    }
    if (loopT == 218) SetBossFieldPosition(enemy, 192.0f, 144.0f);
}

void RunMidbossMain(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_BossTitle              = "Sakuya Izayoi";
        enemy.m_BossLifeCount          = 0;
        enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
        enemy.m_BossTimer              = 0;
        enemy.m_TimerCallbackThreshold = 2400;
        enemy.m_TimerCallbackSub       = SUB_SAKUYA_MIDBOSS_EXIT;
        enemy.m_LifeCallbackThreshold  = 710;
        enemy.m_LifeCallbackSub        = SUB_SAKUYA_MISDIRECTION;
        enemy.m_DeathCallbackSub       = SUB_SAKUYA_MIDBOSS_DEATH;
        enemy.m_CanTakeDamage          = true;
        ctx.SetTimeStopped(false);
        ctx.BulletCancelIntoPointItems();
    }
    if (t < 30) return;

    const int  loopT = (t - 30) % 281;
    const auto pos   = ShootPos(enemy, {0.0f, -12.0f});
    if (loopT < 80 && loopT % 8 == 0) {
        SpawnMidbossDaggerSweep(ctx, pos, (static_cast<float>(loopT) / 8.0f) * 0.31415927f);
    }
    if (loopT == 80) {
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
    if (loopT >= 80 && loopT < 110 && (loopT - 80) % 3 == 0) {
        SpawnMidbossKunaiSweep(ctx, pos,
                               PI - (static_cast<float>(loopT - 80) / 3.0f) * 0.31415927f);
    }
    if (loopT >= 170 && loopT < 250 && (loopT - 170) % 8 == 0) {
        SpawnMidbossDaggerSweep(ctx, pos,
                                PI - (static_cast<float>(loopT - 170) / 8.0f) * 0.31415927f);
    }
    if (loopT >= 250 && loopT < 280 && (loopT - 250) % 3 == 0) {
        SpawnMidbossKunaiSweep(ctx, pos, (static_cast<float>(loopT - 250) / 3.0f) * 0.31415927f);
    }
}

}  // namespace Stage5Detail
