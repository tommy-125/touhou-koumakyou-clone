#include "Scene/Stage1/Stage1Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Scene/Stage1/Stage1Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

using namespace Stage1Detail;
void Stage1Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

Stage1Script::Stage1Script() {
    RegisterBossPhases({
        StageScriptUtil::ConfigId::BossPhase::Stage1RumiaMidboss,
        StageScriptUtil::ConfigId::BossPhase::Stage1RumiaFirstNonspell,
        StageScriptUtil::ConfigId::BossPhase::Stage1NightBird,
        StageScriptUtil::ConfigId::BossPhase::Stage1RumiaSecondNonspell,
        StageScriptUtil::ConfigId::BossPhase::Stage1Demarcation,
    });

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
