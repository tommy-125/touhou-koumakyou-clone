#include "Scene/Stage6/Stage6PatternCommon.hpp"

#include "Enemy/Enemy.hpp"

namespace Stage6Detail {
float RandFloat(float min, float max) {
    return ScriptUtil::RandFloat(min, max);
}

float MirrorAngle(float angle, bool mirrored) {
    return mirrored ? PI - angle : angle;
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetSakuyaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

void SetRemiliaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 160, 163, 164, 161, 162);
}
}  // namespace Stage6Detail
