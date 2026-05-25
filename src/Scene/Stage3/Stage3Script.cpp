#include "Scene/Stage3/Stage3Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace StageUtil = StageScriptUtil;
using namespace Stage3Detail;
}  // namespace
void Stage3Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG3ENM.folder, Anm::STG3ENM.txt, Anm::STG3ENM.offset);
}

Stage3Script::Stage3Script() {
    AddTimedPattern({0, 1}, InitStage3OpeningFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunOpeningFairy(enemy, t); });
    AddTimedPattern({2, 3}, InitStage3SideFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunSideFairy(enemy, t); });
    AddTimedPattern(4, InitStage3Script15Fairy, RunMediumKunai);
    AddTimedPattern(5, InitStage3MediumRingFairy, RunMediumRing);
    AddTimedPattern(6, InitStage3BlueScatterFairy, RunBlueScatter);
    AddTimedPattern(7, InitStage3Script15Fairy, RunFixedDownKunaiFairy);
    AddTimedPattern(8, InitStage3Script15Fairy, RunWhiteRandomBalls);

    AddTimedPattern(SUB_MEILING_MIDBOSS_MAIN, InitMeilingMidboss,
                    [](Enemy& enemy, EnemySubCtx& ctx, int t) {
                        if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
                        if (t == 100) enemy.m_CanTakeDamage = true;
                        if (t >= 130) RunMeilingMidbossPattern(enemy, ctx, t - 130);
                    });
    AddTimedRunOnlyPattern({SUB_MEILING_MIDBOSS_SPELL_A, SUB_MEILING_MIDBOSS_SPELL_B},
                           RunMeilingMidbossSpell);
    AddTimedRunOnlyPattern(SUB_MEILING_MIDBOSS_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Life);
            ctx.TransitionToSub(enemy, SUB_MEILING_MIDBOSS_ESCAPE);
        }
    });
    AddTimedRunOnlyPattern(SUB_MEILING_MIDBOSS_ESCAPE, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            enemy.m_CanTakeDamage = false;
            enemy.m_ShowSpellName = false;
            enemy.m_InSpellcard   = false;
            ctx.BulletCancelIntoPointItems();
        }
        if (t == 130) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
        if (t == 190) enemy.m_Alive = false;
    });
    AddTimedPattern(SUB_MEILING_SUPPORT_FAIRY, InitStage3Script15Fairy, RunMeilingSupportFairy);

    AddTimedPattern(SUB_MEILING_ENTRY, InitMeilingBossEntry,
                    [](Enemy& enemy, EnemySubCtx& ctx, int t) {
                        if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
                        if (t == 120) ctx.TransitionToSub(enemy, SUB_MEILING_FIRST_NONSPELL);
                    });
    AddBossPhasePattern(SUB_MEILING_FIRST_NONSPELL,
                        StageUtil::ConfigId::BossPhase::Stage3MeilingFirstNonspell,
                        RunMeilingFirstNonSpell, nullptr, 50);
    AddBossPhasePattern(SUB_MEILING_RAINBOW_WIND_CHIME,
                        StageUtil::ConfigId::BossPhase::Stage3RainbowWindChime,
                        RunRainbowWindChimeSpell, [](Enemy& enemy, EnemySubCtx& ctx) {
                            enemy.m_LockedShotAngle = 0.87266463f;
                            ctx.StartLerpTo(enemy, 192.0f, 64.0f, 120);
                        });
    AddBossPhasePattern(
        SUB_MEILING_SECOND_NONSPELL,
        StageUtil::ConfigId::BossPhase::Stage3MeilingSecondNonspell, RunMeilingSecondNonSpell,
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
        },
        160);
    AddBossPhasePattern(
        SUB_MEILING_FINAL_NONSPELL, StageUtil::ConfigId::BossPhase::Stage3MeilingFinalNonspell,
        RunMeilingFinalNonSpell,
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
        },
        160);
    AddBossPhasePattern(
        {SUB_MEILING_COLORFUL_RAIN_A, SUB_MEILING_COLORFUL_RAIN_B},
        StageUtil::ConfigId::BossPhase::Stage3ColorfulRain, RunColorfulRainSpell,
        [](Enemy& enemy, EnemySubCtx& ctx) { ctx.StartLerpTo(enemy, 192.0f, 64.0f, 120); });
    AddTimedRunOnlyPattern(SUB_MEILING_EXTREME_TYPHOON, RunGorgeousTyphoonSpell);
    AddTimedRunOnlyPattern(SUB_MEILING_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            enemy.m_CanTakeDamage = false;
            enemy.m_InSpellcard   = false;
            enemy.m_ShowSpellName = false;
            ctx.bullets.ClearAll();
            ctx.lasers.ClearAll();
            enemy.m_Alive = false;
        }
    });
}
