#include "Scene/Stage4/Stage4Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Scene/Stage4/Stage4Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace StageUtil = StageScriptUtil;
using namespace Stage4Detail;
}  // namespace
void Stage4Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG4ENM.folder, Anm::STG4ENM.txt, Anm::STG4ENM.offset);
}

Stage4Script::Stage4Script() {
    AddTimedPattern(0, InitStage4BookFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunBookFairy(enemy, ctx, t, SUB_LIBRARY_FAIRY_BURST);
    });
    AddTimedPattern(10, InitStage4SideBookFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunBookFairy(enemy, ctx, t, SUB_LIBRARY_FAIRY_BURST);
    });
    AddTimedPattern(18, InitStage4BookFairy,
                    [](Enemy& enemy, EnemySubCtx& ctx, int t) { RunBookFairy(enemy, ctx, t, 20); });
    AddTimedPattern(SUB_LIBRARY_FAIRY_BURST, InitStage4BurstFamiliar,
                    [](Enemy& enemy, EnemySubCtx& ctx, int t) {
                        RunBurstFamiliar(enemy, ctx, t, false, false);
                    });
    AddTimedPattern(19, InitStage4BurstFamiliar, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunBurstFamiliar(enemy, ctx, t, false, true);
    });
    AddTimedPattern(20, InitStage4BurstFamiliar, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunBurstFamiliar(enemy, ctx, t, true, false);
    });

    AddTimedPattern({2, 3, 4, 5, 6, 7, 8, 9}, InitStage4SmallFairyEnemy, RunStage4SmallFairy);
    AddTimedPattern(11, InitStage4RingCaster, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunRingCaster(enemy, ctx, t, false);
    });
    AddTimedPattern(13, InitStage4RingCaster, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunRingCaster(enemy, ctx, t, true);
    });
    AddTimedPattern(12, InitStage4DropProxy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power3CancelDie);
        }
    });
    AddTimedPattern(15, InitStage4DropProxy, [](Enemy& enemy, EnemySubCtx&, int t) {
        if (t >= 30) enemy.m_Alive = false;
    });
    AddTimedPattern(14, InitStage4CrystalFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunCrystalFairy(enemy, ctx, t, 60, true, false);
    });
    AddTimedPattern(16, InitStage4CrystalFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunCrystalFairy(enemy, ctx, t, 10, false, false);
    });
    AddTimedPattern(17, InitStage4CrystalFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunCrystalFairy(enemy, ctx, t, 6, true, false);
    });

    AddTimedPattern(SUB_KOAKUMA_MIDBOSS, InitKoakumaMidboss, RunKoakumaMidboss);
    AddTimedRunOnlyPattern(SUB_KOAKUMA_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power8Bomb);
            ctx.TransitionToSub(enemy, SUB_KOAKUMA_ESCAPE);
        }
    });
    AddTimedRunOnlyPattern(SUB_KOAKUMA_ESCAPE, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            enemy.m_CanTakeDamage = false;
            ctx.BulletCancelIntoPointItems();
            ctx.lasers.ClearAll();
            enemy.m_Alive = false;
        }
    });

    AddTimedPattern(SUB_PATCHOULI_ENTRY, InitPatchouliEntry,
                    [](Enemy& enemy, EnemySubCtx& ctx, int t) {
                        if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
                        if (t == 60) ctx.TransitionToSub(enemy, SUB_PATCHOULI_FIRST_NONSPELL);
                    });
    AddBossPhasePattern(SUB_PATCHOULI_FIRST_NONSPELL,
                        StageUtil::ConfigId::BossPhase::Stage4PatchouliFirstNonspell,
                        RunPatchouliFirstNonSpell, [](Enemy& enemy, EnemySubCtx& ctx) {
                            ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
                        });
    AddBossPhasePattern(SUB_PATCHOULI_SECOND_NONSPELL,
                        StageUtil::ConfigId::BossPhase::Stage4PatchouliSecondNonspell,
                        RunPatchouliSecondNonSpell, [](Enemy& enemy, EnemySubCtx& ctx) {
                            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power8);
                            ctx.StartLerpTo(enemy, 192.0f, 128.0f, 120);
                        });
    AddBossPhasePattern(
        SUB_PATCHOULI_PRINCESS_UNDINE, StageUtil::ConfigId::BossPhase::Stage4PrincessUndine,
        RunPrincessUndine,
        [](Enemy& enemy, EnemySubCtx& ctx) { ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120); });
    AddBossPhasePattern(
        SUB_PATCHOULI_SYLPHY_HORN_ADV,
        StageUtil::ConfigId::BossPhase::Stage4SylphyHornAdvanced, RunSylphyHornAdvanced,
        [](Enemy& enemy, EnemySubCtx& ctx) { ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120); });
    AddBossPhasePattern(SUB_PATCHOULI_FINAL_NONSPELL,
                        StageUtil::ConfigId::BossPhase::Stage4FinalNonspell, RunFinalNonSpell,
                        [](Enemy& enemy, EnemySubCtx& ctx) {
                            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power8);
                            ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
                            enemy.m_SecondaryShotAngle = 0.0f;
                        });
    AddBossPhasePattern(SUB_PATCHOULI_PHASE_OUT, StageUtil::ConfigId::BossPhase::Stage4PhaseOut,
                        RunMercuryPoison, [](Enemy& enemy, EnemySubCtx& ctx) {
                            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power8);
                            ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120);
                        });
    AddBossPhasePattern(
        SUB_PATCHOULI_WATER_ELF, StageUtil::ConfigId::BossPhase::Stage4WaterElf, RunWaterElf,
        [](Enemy& enemy, EnemySubCtx& ctx) { ctx.StartLerpTo(enemy, 192.0f, 80.0f, 120); });
    AddTimedRunOnlyPattern(SUB_PATCHOULI_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
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
