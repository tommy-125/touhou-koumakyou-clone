#pragma once

#include <glm/glm.hpp>

#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Stage5Patterns.hpp"

namespace Stage5Detail {
constexpr glm::vec2 SAKUYA_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil = EnemyScriptUtil;

inline glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = SAKUYA_SHOOT_OFFSET) {
    return ScriptUtil::ShootPos(enemy, offset);
}

inline float RankedSpeed1(float speed) {
    return speed + SAKUYA_FINAL_RANK_SPEED;
}

inline float RankedSpeed2(float speed) {
    return speed + SAKUYA_FINAL_RANK_SPEED * 0.5f;
}

inline void SetSakuyaBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

inline void StartSakuyaPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                             int lifeCount, int timerFrames, int nextSub, int lifeThreshold,
                             bool spell) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  nextSub,
                                  lifeThreshold,
                                  nextSub,
                                  spell,
                                  spell,
                                  0,
                                  false,
                                  true,
                              });
}
}  // namespace Stage5Detail