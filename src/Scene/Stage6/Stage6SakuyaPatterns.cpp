#include "Stage6PatternCommon.hpp"

#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"

namespace Stage6Detail {
using EnemyPatternUtil::RandAngle;
using EnemyPatternUtil::SpawnFanAbs;
using EnemyPatternUtil::SpawnRandomArc;
void RunSakuyaMain(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        BossPhaseUtil::StartPhase(enemy, ctx,
                                  {
                                      "Sakuya Izayoi",
                                      -1,
                                      0,
                                      1020,
                                      SUB_SAKUYA_EXIT,
                                      SUB_SAKUYA_DEATH,
                                      1000,
                                      SUB_SAKUYA_ETERNAL_MEEK,
                                      false,
                                      false,
                                      0,
                                      false,
                                      true,
                                  });
        enemy.m_CanTakeDamage = true;
        ctx.SetTimeStopped(false);
    }
    if (t < 30) return;
    const int loopT = (t - 30) % 130;
    const auto pos  = ShootPos(enemy);
    if (loopT < 48 && loopT % 4 == 0) {
        SpawnFanAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f,
                    3.2f, (loopT / 4) * 0.2617994f, 0.2617994f, true);
    }
    if (loopT == 48) {
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
    if (loopT >= 48 && loopT < 64) {
        SpawnFanAbs(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 5, 1, 2.5f, 2.5f,
                    RandAngle(), 0.044879895f, true);
    }
    if (loopT >= 65 && loopT < 113 && (loopT - 65) % 4 == 0) {
        SpawnFanAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f,
                    3.2f, PI - ((loopT - 65) / 4) * 0.2617994f, 0.2617994f, true);
    }
    if (loopT >= 113 && loopT < 129) {
        SpawnFanAbs(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 5, 1, 2.5f, 2.5f,
                    RandAngle(), 0.044879895f, true);
    }
}

void RunEternalMeek(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        BossPhaseUtil::StartPhase(enemy, ctx,
                                  {
                                      "Conjuring \"Eternal Meek\"",
                                      -1,
                                      0,
                                      900,
                                      SUB_SAKUYA_EXIT,
                                      SUB_SAKUYA_DEATH,
                                      -1,
                                      -1,
                                      true,
                                      true,
                                      0,
                                      false,
                                      true,
                                  });
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 4;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0) {
        SpawnRandomArc(ctx, pos, EBulletType::Ball, EBulletColor::Blue, 4, 3.0f, 6.0f, 0.0f,
                       PI);
        SpawnRandomArc(ctx, pos, EBulletType::Ball, EBulletColor::Blue, 12, 3.0f, 5.0f, -PI,
                       0.0f);
    }
}
}  // namespace Stage6Detail