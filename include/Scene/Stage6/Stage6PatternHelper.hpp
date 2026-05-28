#ifndef SCENE_STAGE6_STAGE6_PATTERN_HELPER_HPP
#define SCENE_STAGE6_STAGE6_PATTERN_HELPER_HPP

#include <glm/glm.hpp>

#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage6/Stage6Patterns.hpp"

class Enemy;

namespace Stage6Detail {
namespace ScriptUtil = EnemyScriptUtil;

float     RandFloat(float min, float max);
float     MirrorAngle(float angle, bool mirrored);
glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f});
void      SetSakuyaPoses(Enemy& enemy);
void      SetRemiliaPoses(Enemy& enemy);
}  // namespace Stage6Detail

#endif  // SCENE_STAGE6_STAGE6_PATTERN_HELPER_HPP
