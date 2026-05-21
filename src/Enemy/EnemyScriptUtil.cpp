#include "Enemy/EnemyScriptUtil.hpp"

#include <cstdlib>

#include "Item/ItemManager.hpp"

namespace EnemyScriptUtil {

float RandFloat(float min, float max) {
    return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset) {
    return enemy.m_Pos + offset;
}

void SetDeathEffects(Enemy& enemy, int primary, int secondary) {
    enemy.m_DeathEffectPrimary   = primary;
    enemy.m_DeathEffectSecondary = secondary;
}

void SetBossPoses(Enemy& enemy, int defaults, int farLeft, int farRight, int left, int right) {
    enemy.m_AnmDefault   = defaults;
    enemy.m_AnmFarLeft   = farLeft;
    enemy.m_AnmFarRight  = farRight;
    enemy.m_AnmLeft      = left;
    enemy.m_AnmRight     = right;
    enemy.m_AnmMoveState = 0xff;
}

void DisableBossPoses(Enemy& enemy) {
    enemy.m_AnmDefault   = -1;
    enemy.m_AnmFarLeft   = -1;
    enemy.m_AnmFarRight  = -1;
    enemy.m_AnmLeft      = -1;
    enemy.m_AnmRight     = -1;
    enemy.m_AnmMoveState = 0xff;
}

void DropPowerItems(Enemy& enemy, EnemySubCtx& ctx, int count) {
    for (int i = 0; i < count; i++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
}

void StartRandomMove(Enemy& enemy, const EnemySubCtx& ctx, float speed, int frames) {
    ctx.MoveRandInBounds(enemy);
    ctx.StartLerpDir(enemy, speed, frames);
}

}  // namespace EnemyScriptUtil
