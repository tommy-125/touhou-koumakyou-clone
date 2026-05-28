#ifndef SCENE_STAGE4_STAGE4_PATTERN_HELPER_HPP
#define SCENE_STAGE4_STAGE4_PATTERN_HELPER_HPP

#include <glm/glm.hpp>

#include "Scene/Stage4/Stage4Patterns.hpp"

class Enemy;
struct EnemySubCtx;

namespace Stage4Detail {
glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f});
void      SetPatchouliBossPoses(Enemy& enemy);
void      SpawnAtEnemyFieldPos(const Enemy& enemy, EnemySubCtx& ctx, int subId, int life, int score,
                               int itemDrop);
}  // namespace Stage4Detail

#endif  // SCENE_STAGE4_STAGE4_PATTERN_HELPER_HPP
