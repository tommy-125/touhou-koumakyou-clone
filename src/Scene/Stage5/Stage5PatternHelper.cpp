#include "Scene/Stage5/Stage5PatternHelper.hpp"

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemySubCtx.hpp"

namespace Stage5Detail {
glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset) {
    return ScriptUtil::ShootPos(enemy, offset);
}

float RankedSpeed1(float speed) {
    return speed + SAKUYA_FINAL_RANK_SPEED;
}

float RankedSpeed2(float speed) {
    return speed + SAKUYA_FINAL_RANK_SPEED * 0.5f;
}

void SetSakuyaBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

bool BeginSakuyaSpellAt(Enemy& enemy, EnemySubCtx& ctx, int t, glm::vec2 target, int warmup) {
    if (t == 0) {
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, target.x, target.y, warmup);
    }
    if (t == warmup) enemy.m_CanTakeDamage = true;
    return t >= warmup;
}
}  // namespace Stage5Detail
