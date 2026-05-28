#ifndef SCENE_STAGE3_STAGE3_PATTERN_HELPER_HPP
#define SCENE_STAGE3_STAGE3_PATTERN_HELPER_HPP

#include <glm/glm.hpp>

#include "Enemy/EnemyBulletManager.hpp"
#include "Scene/Stage3/Stage3Patterns.hpp"

class Enemy;
struct EnemySubCtx;

namespace Stage3Detail {
glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f});
void      SetMeilingBossPoses(Enemy& enemy);
void SpawnRandomSpeedRange(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                           int count, float minSpeed, float maxSpeed,
                           bool rotateWithAngle = false);
void SpawnRandomVectorAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                            int count, float maxSpeed, float minSpeed, float accelSpeed,
                            float accelAngle, bool rotateWithAngle = true);
void SpawnRandomDownAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                          int count, float maxSpeed, float minSpeed,
                          bool rotateWithAngle = true);
}  // namespace Stage3Detail

#endif  // SCENE_STAGE3_STAGE3_PATTERN_HELPER_HPP
