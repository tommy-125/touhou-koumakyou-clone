#include "Scene/Stage1/Stage1Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage1/Stage1Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace StageUtil = StageScriptUtil;
}  // namespace

using namespace Stage1Detail;
void Stage1Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

Stage1Script::Stage1Script() {
    AddBossPhase(BossPhaseSpec(StageUtil::ConfigId::BossPhase::Stage1RumiaMidboss,
                               {SUB_MIDBOSS_MAIN}));
    AddBossPhase(BossPhaseSpec(StageUtil::ConfigId::BossPhase::Stage1RumiaFirstNonspell,
                               {SUB_BOSS_PHASE1_INIT, SUB_BOSS_PHASE1_ATTACK_A,
                                SUB_BOSS_PHASE1_ATTACK_B, SUB_BOSS_PHASE1_ATTACK_C,
                                SUB_BOSS_PHASE1_ATTACK_D})
                     .Start([](Enemy& enemy, EnemySubCtx&) {
                         EnemyScriptUtil::SetDeathEffects(enemy, 671, 676);
                     }));
    AddBossPhase(BossPhaseSpec(StageUtil::ConfigId::BossPhase::Stage1NightBird,
                               {SUB_BOSS_PHASE1_SPELL})
                     .Start([](Enemy& enemy, EnemySubCtx& ctx) {
                         ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
                     }));
    AddBossPhase(BossPhaseSpec(StageUtil::ConfigId::BossPhase::Stage1RumiaSecondNonspell,
                               {SUB_BOSS_PHASE2_INIT, SUB_BOSS_PHASE2_ATTACK_A,
                                SUB_BOSS_PHASE2_ATTACK_B, SUB_BOSS_PHASE2_ATTACK_C,
                                SUB_BOSS_PHASE2_ATTACK_D})
                     .Start([](Enemy& enemy, EnemySubCtx& ctx) {
                         EnemyScriptUtil::SetDeathEffects(enemy, 671, 676);
                         StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
                     }));
    AddBossPhase(BossPhaseSpec(StageUtil::ConfigId::BossPhase::Stage1Demarcation,
                               {SUB_BOSS_PHASE2_SPELL})
                     .Start([](Enemy& enemy, EnemySubCtx& ctx) {
                         ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
                     }));

    AddTimedPattern(0, InitStage1SmallFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunStage1SmallFairyA(enemy, t); });
    AddTimedPattern(1, InitStage1SmallFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunStage1SmallFairyB(enemy, t); });
    AddTimedPattern(2, InitStage1MediumFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunStage1MediumFairy(enemy, ctx, t, true);
    });
    AddTimedPattern(3, InitStage1MediumFairy, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunStage1MediumFairy(enemy, ctx, t, false);
    });

    AddTimedPattern({SUB_MIDBOSS_MAIN, SUB_MIDBOSS_ESCAPE, SUB_MIDBOSS_DEATH}, InitRumiaMidbossSub,
                    RunRumiaMidbossSub);

    AddTimedPattern(SUB_BOSS_ENTRY, InitRumiaBossEntry, RunRumiaBossEntry);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE1_INIT, RunRumiaFirstNonspellInit);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE1_ATTACK_A, RunRumiaFirstAttackA);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE1_ATTACK_B, RunRumiaFirstAttackB);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE1_ATTACK_C, RunRumiaFirstAttackC);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE1_ATTACK_D, RunRumiaFirstAttackD);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE1_SPELL, RunNightBird);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE2_INIT, RunRumiaSecondNonspellInit);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE2_ATTACK_A, RunRumiaSecondAttackA);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE2_ATTACK_B, RunRumiaSecondAttackB);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE2_ATTACK_C, RunRumiaSecondAttackC);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE2_ATTACK_D, RunRumiaSecondAttackD);
    AddTimedRunOnlyPattern(SUB_BOSS_PHASE2_SPELL, RunDemarcation);
    AddTimedRunOnlyPattern(SUB_BOSS_DEATH, RunRumiaBossDeath);
}
