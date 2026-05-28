#ifndef SCENE_STAGE5_STAGE5_PATTERN_HELPER_HPP
#define SCENE_STAGE5_STAGE5_PATTERN_HELPER_HPP

#include <glm/glm.hpp>

#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage5/Stage5Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

class Enemy;
struct EnemySubCtx;

namespace Stage5Detail {
constexpr glm::vec2 SAKUYA_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil                    = EnemyScriptUtil;
namespace StageUtil                     = StageScriptUtil;

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = SAKUYA_SHOOT_OFFSET);
float     RankedSpeed1(float speed);
float     RankedSpeed2(float speed);
void      SetSakuyaBossPoses(Enemy& enemy);
void      StartSakuyaPhase(Enemy& enemy, const EnemySubCtx& ctx,
                           StageUtil::ConfigId::BossPhaseId phaseId);
bool      BeginSakuyaSpellAt(Enemy& enemy, EnemySubCtx& ctx, int t,
                             StageUtil::ConfigId::BossPhaseId phaseId, glm::vec2 target,
                             int warmup = 120);
}  // namespace Stage5Detail

#endif  // SCENE_STAGE5_STAGE5_PATTERN_HELPER_HPP
