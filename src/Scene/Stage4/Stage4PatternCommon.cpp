#include "Scene/Stage4/Stage4PatternCommon.hpp"

#include "Anm/AnmDefs.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage4Detail {
constexpr glm::vec2 PATCHOULI_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil                       = EnemyScriptUtil;

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetPatchouliBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, Anm::STG4ENM.offset + 65, Anm::STG4ENM.offset + 69,
                             Anm::STG4ENM.offset + 70, Anm::STG4ENM.offset + 67,
                             Anm::STG4ENM.offset + 68);
}

void SpawnAtEnemyFieldPos(const Enemy& enemy, EnemySubCtx& ctx, int subId, int life, int score,
                          int itemDrop) {
    const glm::vec2 field = Util::ScreenToGameField(enemy.m_Pos);
    ctx.SpawnEnemy(subId, field.x, field.y, life, score, false, itemDrop);
}

}  // namespace Stage4Detail
