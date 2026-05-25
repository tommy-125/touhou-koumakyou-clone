#include "Scene/Stage2/Stage2Script.hpp"

#include <algorithm>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage2/Stage2Patterns.hpp"
#include "Util/Math.hpp"

namespace {
namespace ScriptUtil = EnemyScriptUtil;
using EnemyPatternUtil::AimAngle;
using EnemyPatternUtil::SpawnRandomVarianceCircle;
using namespace Stage2Detail;
}  // namespace
void Stage2Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG2ENM.folder, Anm::STG2ENM.txt, Anm::STG2ENM.offset);
    anm.LoadAnm(Anm::STG2ENM2.folder, Anm::STG2ENM2.txt, Anm::STG2ENM2.offset);
}

Stage2Script::Stage2Script() {
    AddTimedPattern({0, 1, 2, 3, 4}, InitStage2AngledFairy, RunStage2AngledFairy);
    AddTimedRunOnlyPattern(5, RunStage2DeathBurst);
    AddTimedPattern(6, InitStage2AimedFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunStage2AimedFairy(enemy, t); });
    AddTimedPattern(7, InitStage2RandomFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunStage2TimedDespawn(enemy, t); });
    AddTimedPattern({8, 9, 10, 11}, InitStage2ShardFairy,
                    [](Enemy& enemy, EnemySubCtx&, int t) { RunStage2TimedDespawn(enemy, t); });
    AddTimedPattern({12, 13}, InitStage2MediumFairy, RunStage2MediumFairy);

    AddTimedPattern({SUB_DAIYOUSEI_MAIN, SUB_DAIYOUSEI_DEATH, SUB_DAIYOUSEI_ESCAPE},
                    InitDaiyouseiSub, RunDaiyouseiSub);
    AddTimedPattern({SUB_CIRNO_ENTRY, SUB_CIRNO_NONSPELL_INIT, SUB_CIRNO_NONSPELL_ATTACK_A,
                     SUB_CIRNO_NONSPELL_ATTACK_B, SUB_CIRNO_ICICLE_FALL, SUB_CIRNO_PHASE2_INIT,
                     SUB_CIRNO_PREFREEZE_ATTACK_A, SUB_CIRNO_PREFREEZE_ATTACK_B,
                     SUB_CIRNO_PERFECT_FREEZE, SUB_CIRNO_DIAMOND_BLIZZARD, SUB_CIRNO_DEATH},
                    InitCirnoSub, RunCirnoSub);
}
