#include "Scene/Stage3/Stage3Script.hpp"

#include <algorithm>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"
#include "Util/Math.hpp"

namespace {
namespace ScriptUtil = EnemyScriptUtil;
using namespace Stage3Detail;
}  // namespace
void Stage3Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG3ENM.folder, Anm::STG3ENM.txt, Anm::STG3ENM.offset);
}

Stage3Script::Stage3Script() {
    AddPattern({0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                SUB_MEILING_MIDBOSS_MAIN,
                SUB_MEILING_MIDBOSS_SPELL_A,
                SUB_MEILING_MIDBOSS_SPELL_B,
                SUB_MEILING_MIDBOSS_DEATH,
                SUB_MEILING_MIDBOSS_ESCAPE,
                SUB_MEILING_SUPPORT_FAIRY,
                SUB_MEILING_ENTRY,
                SUB_MEILING_FIRST_NONSPELL,
                SUB_MEILING_RAINBOW_WIND_CHIME,
                SUB_MEILING_SECOND_NONSPELL,
                SUB_MEILING_FINAL_NONSPELL,
                SUB_MEILING_COLORFUL_RAIN_A,
                SUB_MEILING_COLORFUL_RAIN_B,
                SUB_MEILING_EXTREME_TYPHOON,
                SUB_MEILING_DEATH},
               InitStage3Sub, RunStage3Sub);
}