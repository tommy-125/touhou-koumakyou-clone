#include "Scene/Stage1/Stage1Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Scene/Stage1/Stage1Patterns.hpp"

using namespace Stage1Detail;
void Stage1Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG1ENM.folder, Anm::STG1ENM.txt, Anm::STG1ENM.offset);
    anm.LoadAnm(Anm::STG1ENM2.folder, Anm::STG1ENM2.txt, Anm::STG1ENM2.offset);
}

Stage1Script::Stage1Script() {
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
    AddTimedPattern(
        {SUB_BOSS_ENTRY, SUB_BOSS_PHASE1_INIT, SUB_BOSS_PHASE1_ATTACK_A, SUB_BOSS_PHASE1_ATTACK_B,
         SUB_BOSS_PHASE1_ATTACK_C, SUB_BOSS_PHASE1_ATTACK_D, SUB_BOSS_PHASE1_SPELL,
         SUB_BOSS_PHASE2_INIT, SUB_BOSS_PHASE2_ATTACK_A, SUB_BOSS_PHASE2_ATTACK_B,
         SUB_BOSS_PHASE2_ATTACK_C, SUB_BOSS_PHASE2_ATTACK_D, SUB_BOSS_PHASE2_SPELL, SUB_BOSS_DEATH},
        InitRumiaBossSub, RunRumiaBossSub);
}
