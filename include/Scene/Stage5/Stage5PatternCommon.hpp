#pragma once

#include <glm/glm.hpp>

#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage5/Stage5Patterns.hpp"

namespace Stage5Detail {
constexpr glm::vec2 SAKUYA_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil                    = EnemyScriptUtil;

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

inline bool BeginSakuyaSpellAt(Enemy& enemy, EnemySubCtx& ctx, int t, const char* title,
                               int lifeCount, int nextSub, glm::vec2 target, int timerFrames = 1800,
                               int warmup = 120) {
    if (t == 0) {
        StartSakuyaPhase(enemy, ctx, title, -1, lifeCount, timerFrames, nextSub, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, target.x, target.y, warmup);
    }
    if (t == warmup) enemy.m_CanTakeDamage = true;
    return t >= warmup;
}
}  // namespace Stage5Detail
