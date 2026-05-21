#pragma once

#include <glm/glm.hpp>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Stage6Patterns.hpp"

namespace Stage6Detail {
namespace ScriptUtil = EnemyScriptUtil;

inline float RandFloat(float min, float max) {
    return ScriptUtil::RandFloat(min, max);
}

inline float MirrorAngle(float angle, bool mirrored) {
    return mirrored ? PI - angle : angle;
}

inline glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f}) {
    return ScriptUtil::ShootPos(enemy, offset);
}

inline void SetSakuyaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

inline void SetRemiliaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 160, 163, 164, 161, 162);
}
}  // namespace Stage6Detail