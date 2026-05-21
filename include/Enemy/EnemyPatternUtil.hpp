#ifndef ENEMY_PATTERN_UTIL_HPP
#define ENEMY_PATTERN_UTIL_HPP

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"

namespace EnemyPatternUtil {

constexpr float PI = 3.14159265f;

inline float RandAngle() {
    return EnemyScriptUtil::RandFloat(-PI, PI);
}

inline float AimAngle(glm::vec2 from, glm::vec2 to) {
    return std::atan2(to.y - from.y, to.x - from.x);
}

inline float AimAngleToPlayer(const Enemy& enemy, const EnemySubCtx& ctx) {
    return AimAngle(enemy.m_Pos, ctx.playerPos);
}

inline float EclStackSpeed(int stack, int stacks, float speed1, float speed2) {
    if (stacks <= 1) return speed1;
    return speed1 - (speed1 - speed2) * static_cast<float>(stack) /
                        static_cast<float>(stacks);
}

inline float RankedSpeed(float speed, float rankSpeed) {
    return speed + rankSpeed;
}

inline float RankedLowSpeed(float speed, float rankSpeed) {
    return speed + rankSpeed * 0.5f;
}

inline void SpawnRandomArc(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                           EBulletColor color, int count, float minSpeed, float maxSpeed,
                           float minAngle, float maxAngle, bool rotate = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1,
                                EnemyScriptUtil::RandFloat(minSpeed, maxSpeed),
                                EnemyScriptUtil::RandFloat(minAngle, maxAngle), false, 0.0f,
                                0, rotate);
    }
}

inline void SpawnRandomCircle(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                              EBulletColor color, int count, float minSpeed, float maxSpeed,
                              bool rotate = false) {
    SpawnRandomArc(ctx, pos, type, color, count, minSpeed, maxSpeed,
                   -PI, PI, rotate);
}

inline void SpawnRandomVarianceCircle(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                                      EBulletColor color, int count, float speed,
                                      float variance = 0.0f, bool rotate = false) {
    for (int i = 0; i < count; i++) {
        const float bulletSpeed =
            std::max(0.1f, speed + EnemyScriptUtil::RandFloat(-variance, variance));
        ctx.bullets.SpawnCircle(pos, type, color, 1, bulletSpeed,
                                EnemyScriptUtil::RandFloat(-PI, PI), false, 0.0f, 0, rotate);
    }
}

inline void SpawnFanAbs(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                        int ways, int stacks, float speed1, float speed2, float baseAngle,
                        float spread, bool rotate = false) {
    for (int s = 0; s < stacks; s++) {
        const float speed = EclStackSpeed(s, stacks, speed1, speed2);
        const float start = baseAngle - spread * static_cast<float>(ways - 1) * 0.5f;
        for (int i = 0; i < ways; i++) {
            ctx.bullets.SpawnCircle(pos, type, color, 1, speed, start + spread * i, false,
                                    0.0f, 0, rotate);
        }
    }
}

inline float FanDelta(int count, int index, float spread) {
    int   step  = (count & 1) ? (index + 1) / 2 : index / 2;
    float delta = static_cast<float>(step) * spread;
    if (!(count & 1)) delta += spread * 0.5f;
    return (index & 1) ? -delta : delta;
}

inline void SpawnFanAbsolute(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                             EBulletColor color, int ways, int stacks, float speed1,
                             float speed2, float baseAngle, float spread,
                             bool rotateWithAngle = false,
                             bool bounceTopAndSides = false, int bounceMax = 0) {
    for (int s = 0; s < stacks; s++) {
        const float speed = EclStackSpeed(s, stacks, speed1, speed2);
        for (int i = 0; i < ways; i++) {
            ctx.bullets.SpawnCircle(pos, type, color, 1, speed,
                                    baseAngle + FanDelta(ways, i, spread), false, 0.0f, 0,
                                    rotateWithAngle, {0.0f, 0.0f}, 0, 0, 1.0f, {},
                                    bounceTopAndSides, bounceMax, speed);
        }
    }
}

inline void SpawnAimedStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                            EBulletColor color, int count, int stacks, float speed1,
                            float speed2, float spread = 0.0f, float aimOffset = 0.0f,
                            bool rotate = false) {
    if (spread == 0.0f) {
        for (int s = 0; s < stacks; s++) {
            const float speed = EclStackSpeed(s, stacks, speed1, speed2);
            ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, type, color, count, speed,
                                         aimOffset, false, 0.0f, {}, rotate);
        }
        return;
    }
    SpawnFanAbs(ctx, pos, type, color, count, stacks, speed1, speed2,
                AimAngle(pos, ctx.playerPos) + aimOffset, spread, rotate);
}

inline void SpawnAimedCircleStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                                  EBulletColor color, int count, int stacks, float speed1,
                                  float speed2, float aimOffset = 0.0f,
                                  bool rotate = false) {
    SpawnAimedStack(ctx, pos, type, color, count, stacks, speed1, speed2, 0.0f,
                    aimOffset, rotate);
}

inline void SpawnAimedCircleLinearStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                                        EBulletColor color, int count, int stacks,
                                        float speed1, float speed2,
                                        float aimOffset = 0.0f,
                                        bool rotateWithAngle = false,
                                        float acceleration = 0.0f,
                                        int accelerationFrames = 0,
                                        float angularVelocity = 0.0f,
                                        int angularVelocityFrames = 0) {
    for (int s = 0; s < stacks; s++) {
        const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
        const float speed = speed1 + (speed2 - speed1) * lerp;
        ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, type, color, count, speed, aimOffset,
                                     false, acceleration, accelerationFrames, {},
                                     rotateWithAngle, angularVelocity, angularVelocityFrames);
    }
}

inline void SpawnCircleStackAbs(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                                EBulletColor color, int count, int stacks, float speed1,
                                float speed2, float baseAngle,
                                float stackAngleOffset = 0.0f, bool rotate = false,
                                float acceleration = 0.0f, int accelerationFrames = 0,
                                float angularVelocity = 0.0f,
                                int angularVelocityFrames = 0,
                                bool bounceTopAndSides = false, int bounceMax = 0) {
    for (int s = 0; s < stacks; ++s) {
        const float speed = EclStackSpeed(s, stacks, speed1, speed2);
        ctx.bullets.SpawnCircle(pos, type, color, count, speed,
                                baseAngle + static_cast<float>(s) * stackAngleOffset, false,
                                acceleration, accelerationFrames, rotate, {0.0f, 0.0f}, 0, 0,
                                1.0f, {}, bounceTopAndSides, bounceMax, speed,
                                angularVelocity, angularVelocityFrames);
    }
}

inline void SpawnCircleStackEcl(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                                EBulletColor color, int count, int stacks, float speed1,
                                float speed2, float baseAngle, bool rotateWithAngle,
                                bool bounceTopAndSides = false, int bounceMax = 0,
                                float stackAngleOffset = 0.0f) {
    SpawnCircleStackAbs(ctx, pos, type, color, count, stacks, speed1, speed2, baseAngle,
                        stackAngleOffset, rotateWithAngle, 0.0f, 0, 0.0f, 0,
                        bounceTopAndSides, bounceMax);
}

inline void SpawnAcceleratingRing(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type,
                                  EBulletColor color, int count, float speed, float baseAngle,
                                  float acceleration, int frames, bool rotate = false,
                                  float angularVelocity = 0.0f,
                                  bool vectorAcceleration = false,
                                  float vectorAccelerationAngle = 0.0f) {
    const glm::vec2 vectorAccel =
        vectorAcceleration
            ? glm::vec2{std::cos(vectorAccelerationAngle) * acceleration,
                        std::sin(vectorAccelerationAngle) * acceleration}
            : glm::vec2{0.0f, 0.0f};
    ctx.bullets.SpawnCircle(pos, type, color, count, speed, baseAngle, false,
                            vectorAcceleration ? 0.0f : acceleration,
                            vectorAcceleration ? 0 : frames, rotate, vectorAccel,
                            vectorAcceleration ? frames : 0, 0, 1.0f, {}, false, 0, speed,
                            angularVelocity, angularVelocity == 0.0f ? 0 : frames);
}

}  // namespace EnemyPatternUtil

#endif  // ENEMY_PATTERN_UTIL_HPP
