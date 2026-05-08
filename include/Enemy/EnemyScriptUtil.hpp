#ifndef ENEMY_SCRIPT_UTIL_HPP
#define ENEMY_SCRIPT_UTIL_HPP

#include <glm/glm.hpp>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemySubCtx.hpp"

namespace EnemyScriptUtil {

float RandFloat(float min, float max);
glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {});

void SetDeathEffects(Enemy& enemy, int primary, int secondary);
void SetBossPoses(Enemy& enemy, int defaults, int farLeft, int farRight, int left, int right);
void DropPowerItems(Enemy& enemy, EnemySubCtx& ctx, int count);
void StartRandomMove(Enemy& enemy, const EnemySubCtx& ctx, float speed = 3.0f,
                     int frames = 60);

}  // namespace EnemyScriptUtil

#endif  // ENEMY_SCRIPT_UTIL_HPP
