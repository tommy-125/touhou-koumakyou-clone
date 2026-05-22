#include "Scene/Stage4/Stage4Script.hpp"

#include <algorithm>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage4/Stage4Patterns.hpp"
#include "Util/Math.hpp"

namespace {
namespace ScriptUtil = EnemyScriptUtil;
using namespace Stage4Detail;
}  // namespace
void Stage4Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG4ENM.folder, Anm::STG4ENM.txt, Anm::STG4ENM.offset);
}

Stage4Script::Stage4Script() {
    AddPattern({0,
                18,
                10,
                SUB_LIBRARY_FAIRY_BURST,
                19,
                20,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                11,
                13,
                12,
                15,
                14,
                16,
                17,
                SUB_KOAKUMA_MIDBOSS,
                SUB_KOAKUMA_DEATH,
                SUB_KOAKUMA_ESCAPE,
                SUB_PATCHOULI_ENTRY,
                SUB_PATCHOULI_FIRST_NONSPELL,
                SUB_PATCHOULI_SECOND_NONSPELL,
                SUB_PATCHOULI_PRINCESS_UNDINE,
                SUB_PATCHOULI_SYLPHY_HORN_ADV,
                SUB_PATCHOULI_FINAL_NONSPELL,
                SUB_PATCHOULI_PHASE_OUT,
                SUB_PATCHOULI_WATER_ELF,
                SUB_PATCHOULI_DEATH},
               InitStage4Sub, RunStage4Sub);
}