#include <algorithm>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
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

void InitKoakumaMidboss(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitBossEntry(
        enemy, ctx,
        StageUtil::LoadBossEntryConfig(StageUtil::ConfigId::BossEntry::Stage4KoakumaMidboss));
    enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
    enemy.m_TimerCallbackThreshold = 2400;
    enemy.m_TimerCallbackSub       = SUB_KOAKUMA_ESCAPE;
    enemy.m_DeathCallbackSub       = SUB_KOAKUMA_DEATH;
}

void RunKoakumaMidboss(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ctx.KillAllNonBossEnemies();
        ctx.BulletCancelIntoPointItems();
        enemy.m_Pos = Util::GameFieldToScreen(192.0f, 128.0f);
    }
    if (t == 30) enemy.m_CanTakeDamage = true;

    if (t >= 30) {
        const int loopT = (t - 30) % 441;
        if (loopT == 0 || loopT == 250) enemy.m_LockedShotAngle = RandAngle();

        const auto spawnBubbleRing = [&](int volley, float dir) {
            ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Bubble, EBulletColor::DarkRed, 8,
                                    1.2f, enemy.m_LockedShotAngle + dir * volley, false, 0.024f,
                                    99999, false);
        };

        if (loopT < 160 && loopT % 20 == 0) {
            spawnBubbleRing(loopT / 20, 0.19634955f);
        }
        if (loopT == 160) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
        if (loopT == 160 || loopT == 190 || loopT == 410 || loopT == 440) {
            SpawnAtEnemyFieldPos(enemy, ctx, SUB_LIBRARY_FAIRY_BURST, 1000, 10,
                                 static_cast<int>(ItemType::PowerSmall));
        }
        if (loopT >= 250 && loopT < 410 && (loopT - 250) % 20 == 0) {
            spawnBubbleRing((loopT - 250) / 20, -0.19634955f);
        }
    }
}

}  // namespace Stage4Detail
