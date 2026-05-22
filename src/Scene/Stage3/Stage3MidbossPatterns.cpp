#include <cmath>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"
#include "Util/Math.hpp"

namespace Stage3Detail {
namespace ScriptUtil = EnemyScriptUtil;

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f});
void SpawnRandomSpeedRange(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                           int count, float minSpeed, float maxSpeed, bool rotateWithAngle = false);

void RunMeilingMidbossPattern(Enemy& enemy, EnemySubCtx& ctx, int frame) {
    static constexpr int BLUE_CYCLE_FRAMES = 230;
    static constexpr int RED_CYCLE_FRAMES  = 120;
    const int            pairT             = frame % (BLUE_CYCLE_FRAMES + RED_CYCLE_FRAMES);

    if (pairT < BLUE_CYCLE_FRAMES) {
        if (pairT >= 20 && pairT < 84 && (pairT - 20) % 8 == 0) {
            const glm::vec2 pos = ShootPos(enemy, {ScriptUtil::RandFloat(-16.0f, 16.0f),
                                                   ScriptUtil::RandFloat(-16.0f, 16.0f)});
            SpawnRandomSpeedRange(pos, ctx, EBulletType::Ball, EBulletColor::Blue, 16, 1.7f, 3.0f);
        }
        if (pairT == 124) ScriptUtil::StartRandomMove(enemy, ctx, 5.0f, 50);
    } else {
        const int redT = pairT - BLUE_CYCLE_FRAMES;
        if (redT == 0) {
            ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::RingBall,
                                    EBulletColor::Red, 76, 1.1f);
        }
        if (redT == 10) ScriptUtil::StartRandomMove(enemy, ctx, 5.0f, 50);
    }
}

void RunMeilingMidbossSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_CanTakeDamage          = false;
        enemy.m_ShowSpellName          = true;
        enemy.m_InSpellcard            = true;
        enemy.m_BossTitle              = "Flower Sign \"Gorgeous Sweet Flower\"";
        enemy.m_BossTimer              = 0;
        enemy.m_TimerCallbackThreshold = 1200;
        enemy.m_TimerCallbackSub       = SUB_MEILING_MIDBOSS_ESCAPE;
        enemy.m_DeathCallbackSub       = SUB_MEILING_MIDBOSS_DEATH;
        enemy.m_LockedShotAngle        = ScriptUtil::RandFloat(-PI, PI);
        enemy.m_SecondaryShotAngle     = ScriptUtil::RandFloat(-PI, PI);
        ctx.BulletCancelIntoPointItems();
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int rel = t - 120;
    if (rel % 6 == 0) {
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Shard,
                                EBulletColor::Yellow, 4, 2.0f, enemy.m_LockedShotAngle, false, 0.0f,
                                0, true);
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Shard,
                                EBulletColor::Yellow, 4, 2.0f, enemy.m_SecondaryShotAngle, false,
                                0.0f, 0, true);
        enemy.m_LockedShotAngle += 0.1134464f;
        enemy.m_SecondaryShotAngle -= 0.1308997f;
    }

    if (rel % 80 == 0) {
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy, {0.0f, 0.0f}), ctx.playerPos,
                                     EBulletType::Shard, EBulletColor::Red, 42, 1.2f, 0.0f, false,
                                     0.0f, 0, {}, true);
    }
}

}  // namespace Stage3Detail
