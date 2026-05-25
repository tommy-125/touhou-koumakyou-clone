#include "Scene/Stage3/Stage3PatternCommon.hpp"

#include <cmath>

#include "Anm/AnmDefs.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage3Detail {
constexpr glm::vec2 MEILING_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil                     = EnemyScriptUtil;

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetMeilingBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, Anm::STG3ENM.offset + 64, Anm::STG3ENM.offset + 65,
                             Anm::STG3ENM.offset + 65, Anm::STG3ENM.offset + 65,
                             Anm::STG3ENM.offset + 65);
}

void SpawnRandomSpeedRange(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                           int count, float minSpeed, float maxSpeed, bool rotateWithAngle) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, ScriptUtil::RandFloat(minSpeed, maxSpeed),
                                ScriptUtil::RandFloat(-PI, PI), false, 0.0f, 0, rotateWithAngle);
    }
}

void SpawnRandomVectorAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                            int count, float maxSpeed, float minSpeed, float accelSpeed,
                            float accelAngle, bool rotateWithAngle) {
    const glm::vec2 accel = {std::cos(accelAngle) * accelSpeed, std::sin(accelAngle) * accelSpeed};
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, ScriptUtil::RandFloat(minSpeed, maxSpeed),
                                ScriptUtil::RandFloat(-PI, PI), true, 0.0f, 0, rotateWithAngle,
                                accel, 9999, 12, 0.5f);
    }
}

void SpawnRandomDownAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                          int count, float maxSpeed, float minSpeed, bool rotateWithAngle) {
    SpawnRandomVectorAccel(pos, ctx, type, color, count, maxSpeed, minSpeed, 0.027f, Util::HALF_PI,
                           rotateWithAngle);
}

}  // namespace Stage3Detail
